#ifndef NODE_H
#define NODE_H

#include <Arduino.h>
#include <cstdint>

#include <aim_can_driver.h>
#include <aim_network.h>
#include <aim_safety.h>

#include "pinouts.h"

// Node-level identity and interface configuration lives in this file.
// kSerialBaud is omitted until the power board's UART pins are defined in
// pinouts.h (they are still TBD).
namespace node {
constexpr char     kName[]      = "POWER_MODULE";
constexpr uint32_t kCanBaud     = 500000U;
constexpr uint32_t kSerialBaud   = 38400U;
}  // namespace node

// Application logic entry points.
void nodeInit();
void nodeUpdate(uint32_t nowMs);
void nodeServiceCanTx(uint32_t nowMs, AimNetwork& aim);
void nodeOnRx(const aim::Msg& m, uint32_t nowMs);

aim::NodeState nodeCurrentState();
uint16_t nodeErrorBits();

// Read-only accessors for power telemetry
uint32_t nodeGetBatteryMv();
uint32_t nodeGet5vMv();
uint32_t nodeGetGseMv();
int32_t nodeGet3v3CurrentMa();
int32_t nodeGet24vCurrentMa();

#ifndef FLIGHT_BUILD
#include <aim_console.h>
const AimConsoleHook* nodeConsoleHooks(uint8_t& count);
#endif

#endif  // NODE_H
