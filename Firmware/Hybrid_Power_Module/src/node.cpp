#include "node.h"

#include <logger.h>
#include <aim_job.h>

// Quiescent voltage and scaling state.
static uint32_t s_batteryMv = 3700U;
static uint32_t s_5vMv = 0U;
static uint32_t s_gseMv = 0U;
static int32_t s_i3v3Ma = 0;
static int32_t s_i24vMa = 0;

static constexpr uint32_t kLowPowerThresholdMv = 3400U;
static bool s_lowPowerState = false;
static aim::Job s_telemetryJob{1000U};

void nodeInit(uint32_t nowMs) {
  (void)nowMs;

  pinMode(pins::kDebugLed, OUTPUT);
  digitalWrite(pins::kDebugLed, LOW);

  // Enable 3.3V ACS current sensor power (PA4)
  pinMode(pins::k3V3AcsPower, OUTPUT);
  digitalWrite(pins::k3V3AcsPower, HIGH);

  // Keep 5V ACS current sensor power disabled (PA6)
  // !!! DO NOT POWER ON (using wrong current sense could break mcu)
  pinMode(pins::k5VAcsPower, OUTPUT);
  digitalWrite(pins::k5VAcsPower, LOW);

  // Configure ADC pins as inputs
  pinMode(pins::kBatterySense, INPUT);
  pinMode(pins::k5VSense, INPUT);
  pinMode(pins::kGseSense, INPUT);
  pinMode(pins::k3V3CurrentSense, INPUT);
  pinMode(pins::k24VCurrentSense, INPUT);

  LOG_INFO("Power telemetry pins initialized");
}

void nodeUpdate(uint32_t schedulerNowMs) {
  (void)schedulerNowMs;

  // Sample voltages using integer division
  s_batteryMv = (static_cast<uint32_t>(analogRead(pins::kBatterySense)) * 13200U) / 4095U;
  s_5vMv = (static_cast<uint32_t>(analogRead(pins::k5VSense)) * 6600U) / 4095U;
  s_gseMv = (static_cast<uint32_t>(analogRead(pins::kGseSense)) * 36300U) / 4095U;

  // 3.3V current sensor (U4: ACS37041-010B3, 1.65V zero point, 132 mV/A)
  int32_t v3v3IMv = (static_cast<int32_t>(analogRead(pins::k3V3CurrentSense)) * 3300) / 4095;
  s_i3v3Ma = ((v3v3IMv - 1650) * 1000) / 132;

  // 24V current sensor (U6: INA187A1, 20 V/V gain across 100mOhm shunt -> 2 V/A -> 2 mV/mA)
  int32_t v24vIMv = (static_cast<int32_t>(analogRead(pins::k24VCurrentSense)) * 3300) / 4095;
  s_i24vMa = v24vIMv / 2;
}

void nodeServiceCanTx(uint32_t schedulerNowMs, AimNetwork& aim) {
  if (s_telemetryJob.due(schedulerNowMs)) {
    // 1. Publish battery voltage
    aim::Msg voltMsg = {};
    voltMsg.cls = aim::Class::Sensor;
    voltMsg.subject = aim::subject::BattVolt;
    voltMsg.setSensorValue(static_cast<int32_t>(s_batteryMv));
    (void)aim.send(voltMsg);

    // 2. Check low power threshold and broadcast Event
    const bool isLow = (s_batteryMv < kLowPowerThresholdMv);
    if (isLow != s_lowPowerState) {
      s_lowPowerState = isLow;
      aim::Msg lpMsg = {};
      lpMsg.cls = aim::Class::Event;
      lpMsg.subject = aim::subject::LowPower;
      lpMsg.b[0] = s_lowPowerState ? 1U : 0U;
      (void)aim.send(lpMsg);
    }
  }
}

void nodeOnRx(const aim::Msg& m, uint32_t nowMs) {
  (void)m;
  (void)nowMs;
}

aim::NodeState nodeCurrentState() {
  return aim::NodeState::Nominal;
}

uint16_t nodeErrorBits() {
  return 0U;
}

// Accessor definitions
uint32_t nodeGetBatteryMv() { return s_batteryMv; }
uint32_t nodeGet5vMv() { return s_5vMv; }
uint32_t nodeGetGseMv() { return s_gseMv; }
int32_t nodeGet3v3CurrentMa() { return s_i3v3Ma; }
int32_t nodeGet24vCurrentMa() { return s_i24vMa; }

#ifndef FLIGHT_BUILD
static void hookPowerSnapshot(Stream& out) {
  out.print("V_BATT=");
  out.print(s_batteryMv);
  out.println(" mV");

  out.print("5V_PSU=");
  out.print(s_5vMv);
  out.println(" mV");

  out.print("24V_GSE=");
  out.print(s_gseMv);
  out.println(" mV");

  out.print("I_3V3=");
  out.print(s_i3v3Ma);
  out.println(" mA");

  out.print("I_24V=");
  out.print(s_i24vMa);
  out.println(" mA");

  out.println("I_5V=disabled (HW issue - unpowered)");
}

static const AimConsoleHook s_consoleHooks[] = {
  {'p', "power snapshot", hookPowerSnapshot},
};

const AimConsoleHook* nodeConsoleHooks(uint8_t& count) {
  count = sizeof(s_consoleHooks) / sizeof(s_consoleHooks[0]);
  return s_consoleHooks;
}
#endif
