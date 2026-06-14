#pragma once
#include <Arduino.h>   // STM32 variant pin macros (PB8, PB9, ...)
#include <cstdint>

// pinouts.h — Power Module (STM32F103CB) pin map.
// Values are MCU-native STM32 Arduino pin macros (e.g. PB9 = 25).
// Pin assignments are TBD — fill in as the board hardware firms up.
namespace pins {

// --- CAN ---
constexpr uint8_t kCanRx = PB8;
constexpr uint8_t kCanTx = PB9;

// --- Flash (SPI) ---  TBD
// --- Power sensing ---  TBD
// --- LEDs ---  TBD

}  // namespace pins
