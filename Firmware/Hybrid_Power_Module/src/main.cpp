#include "node.h"

#include <IWatchdog.h>
#include <logger.h>

static constexpr uint32_t kWatchdogTimeoutUs  = 2000000U;
static constexpr uint8_t  kMaxRxFramesPerLoop = 8U;

static AimCanDriver g_canHw(node::kCanBaud, NODE_CAN_BUS);
static AimNetwork g_aim(&g_canHw, aim::Source::Power);

// No diagnostic serial yet: the power board's UART pins are still TBD in
// pinouts.h, so g_logger stays null and LOG_* macros no-op. Add a Logger on a
// SoftwareSerial here once the serial pins are defined (see GPS_Module).

void serviceCanRx(void) {
  // Bounded RX drain. receive() disciplines the local clock on TimeSync.
  for (uint8_t i = 0U; i < kMaxRxFramesPerLoop; i++) {
    aim::Msg m = {};
    if (!g_aim.receive(m)) {
      break;
    }
  }
}

void setup(void) {
  IWatchdog.begin(kWatchdogTimeoutUs);

  // Power is a Sensor/State publisher and TimeSync consumer.
  if (!g_aim.begin(aim::classBit(aim::Class::Time) |
                   aim::classBit(aim::Class::Heartbeat))) {
    LOG_ERROR("CAN init failed");
  }

  nodeInit(millis());
}

void loop(void) {
  const uint32_t schedulerNowMs = millis();

  // Core work runs every loop. Flight recorder + console get added with the
  // flash stack once the flash pins are defined (see GPS_Module for the pattern).
  serviceCanRx();
  nodeUpdate(schedulerNowMs);
  nodeServiceCanTx(schedulerNowMs, g_aim);
  g_aim.service(aim::NodeState::Nominal, 0U);   // heartbeat fills bus silence

  IWatchdog.reload();
}
