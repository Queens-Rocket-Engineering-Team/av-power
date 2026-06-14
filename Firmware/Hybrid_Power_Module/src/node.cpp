#include "node.h"

#include <logger.h>
#include <aim_job.h>

// Simulated battery voltage. Will be replaced by real ADC reads.
static uint32_t s_batteryMv = 3700U; 
static constexpr uint32_t kLowPowerThresholdMv = 3400U;
static bool s_lowPowerState = false;
static aim::Job s_telemetryJob{1000U};

void nodeInit(uint32_t nowMs) {
  // TODO: bring up voltage/current sensing and rail-enable control once the
  // board pinout firms up and the drivers exist.
  (void)nowMs;
}

void nodeUpdate(uint32_t schedulerNowMs) {
  // TODO: sample rails and run the power state machine.
  (void)schedulerNowMs;
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
