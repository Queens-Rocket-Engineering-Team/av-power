#include "node.h"

#include <logger.h>

// Scaffold only: this node heartbeats and consumes TimeSync. The bodies below
// fill in as the power board's drivers land — keep application logic here, not
// in main.cpp (see the firmware node conventions in the workspace CLAUDE.md).

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
  // TODO: publish bus voltage / current Sensor frames and power State.
  (void)schedulerNowMs;
  (void)aim;
}

void nodeOnRx(const aim::Msg& m, uint32_t nowMs) {
  // TODO: handle cross-node events once subjects are defined.
  (void)m;
  (void)nowMs;
}
