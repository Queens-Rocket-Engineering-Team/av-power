#include "node.h"

#include <IWatchdog.h>
#include <logger.h>
#include <SoftwareSerial.h>
#include <SPI.h>

#include <aim_file_system.h>
#include <aim_flight_recorder.h>
#ifndef FLIGHT_BUILD
#include <aim_console.h>
#endif

static constexpr uint32_t kWatchdogTimeoutUs  = 2000000U;
static constexpr uint8_t  kMaxRxFramesPerLoop = 8U;

static AimCanHardware g_canHw(node::kCanBaud, CAN1);
static AimNetwork g_aim(&g_canHw, node::kSource);
static SoftwareSerial g_serial(pins::kSerialRx, pins::kSerialTx);
static Logger g_log(g_serial, static_cast<uint8_t>(node::kSource), LogLevel::INFO);

// Flash on SPI2: MOSI=PB15, MISO=PB14, SCLK=PB13, CS=PB12 (kFlashReset in pinouts.h).
static SPIClass g_flashSpi(pins::kSpiMosi, pins::kSpiMiso, pins::kSpiSclk);
static SpiNorFlashDriver g_flashDriver(pins::kFlashReset, g_flashSpi);
static AimFileSystem g_fs(&g_flashDriver);
static AimFlightRecorder g_recorder(g_fs, kLogCols, kLogOriginRefresh, kLogMaxSize, kLogHeaders);

static void serviceCanRx(void) {
  const uint32_t nowMs = millis();
  for (uint8_t i = 0U; i < kMaxRxFramesPerLoop; i++) {
    aim::Msg m = {};
    if (!g_aim.receive(m)) break;
    nodeOnRx(m, nowMs);
  }
}

#ifndef FLIGHT_BUILD
static void hookStatus(Stream& out) {
  out.print("name=");
  out.print(node::kName);
  out.print(" logMask=0x");
  out.print(static_cast<unsigned>(g_log.filterMask()), HEX);
  out.print(" syncedMs=");
  out.print(static_cast<unsigned long>(g_aim.syncedMillis()));
  out.print(" version=");
  out.print(aim::kNetworkVersionString);
  out.print(" schema=");
  out.print(static_cast<unsigned>(aim::kSchemaVersion));
  out.print(" build=");
  out.print(__DATE__);
  out.print(" ");
  out.println(__TIME__);
}
#endif  // FLIGHT_BUILD

void setup(void) {
  g_serial.begin(node::kSerialBaud);
  g_logger = &g_log;
  LOG_INFO("Boot %s source=%u", node::kName, static_cast<unsigned>(node::kSource));
  IWatchdog.begin(kWatchdogTimeoutUs);
  LOG_INFO("Watchdog ready");

  // Power is a Sensor/State publisher and TimeSync consumer.
  if (!g_aim.begin(aim::classBit(aim::Class::Time) |
                   aim::classBit(aim::Class::Heartbeat))) {
    LOG_ERROR("CAN init failed");
  }

  if (!g_fs.begin()) {
    LOG_WARN("Filesystem mount failed");
  } else if (!g_recorder.begin()) {
    LOG_WARN("Recorder init failed");
  } else {
    LOG_INFO("Flash ready");
  }

#ifndef FLIGHT_BUILD
  uint8_t nodeHookCount = 0U;
  const AimConsoleHook* nodeHooks = nodeConsoleHooks(nodeHookCount);

  static AimConsoleHook combinedHooks[8];
  uint8_t totalHooks = 0;
  combinedHooks[totalHooks++] = {'s', "status", hookStatus};
  for (uint8_t i = 0; i < nodeHookCount && totalHooks < 8; i++) {
    combinedHooks[totalHooks++] = nodeHooks[i];
  }
  aimConsoleInit(g_serial, g_fs, g_recorder, node::kName, combinedHooks, totalHooks);
#endif

  nodeInit();

#ifndef FLIGHT_BUILD
  g_serial.println("Console ready. d=enter debug");
#endif
}

void loop(void) {
  const uint32_t nowMs = millis();

  serviceCanRx();
  nodeUpdate(nowMs);
  nodeServiceCanTx(nowMs, g_aim);
  g_aim.service(nowMs, nodeCurrentState(), nodeErrorBits());

#ifndef FLIGHT_BUILD
  aimConsoleService();
#endif

  IWatchdog.reload();
}
