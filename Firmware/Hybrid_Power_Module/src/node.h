#ifndef NODE_H
#define NODE_H

#include <Arduino.h>
#include <cstdint>

#include <aim_network.h>

class AimFlightRecorder;

#include "pinouts.h"

// Node-level identity and interface configuration lives in this file.
namespace node {
constexpr char        kName[]     = "POWER_MODULE";
constexpr aim::Source kSource     = aim::Source::Power;
constexpr uint32_t    kCanBaud    = 1000000U;
constexpr uint32_t    kSerialBaud = 38400U;
}  // namespace node

static constexpr uint8_t  kLogCols           = 4U;
static constexpr uint16_t kLogOriginRefresh  = 100U;
static constexpr uint32_t kLogMaxSize        = 0;
static const char* const  kLogHeaders[kLogCols] = {"time", "batteryMv", "5vMv", "gseMv"};

// Application logic entry points.
void nodeInit();
void nodeUpdate(uint32_t nowMs);
void nodeServiceLog(uint32_t nowMs, AimFlightRecorder& recorder);
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
