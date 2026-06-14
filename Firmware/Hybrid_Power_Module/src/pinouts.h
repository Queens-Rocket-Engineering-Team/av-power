#ifndef PINOUTS_H
#define PINOUTS_H

#include <Arduino.h> // needed for PB/A# pin assignment
#include <cstdint>

// pinouts.h - APOLLO Hybrid Power Module V1.0 2025/2026 (STM32F103CBT6) pin map.
namespace pins {

// --- Serial (USB-UART Bridge; TX/RX are swapped on the PCB) ---
//  !!! Fix in next board spin !!!
//  Use software serial
constexpr uint8_t kSerialTx = PB10; // USART_TX -> USB_RX
constexpr uint8_t kSerialRx = PB11; // USART_RX <- USB_TX

// --- CAN bus ---
constexpr uint8_t kCanRx = PB8;
constexpr uint8_t kCanTx = PB9;

// --- Flash interface (SPI) ---
constexpr uint8_t kFlashReset = PB12;
constexpr uint8_t kSpiSclk    = PB13;
constexpr uint8_t kSpiMiso    = PB14;
constexpr uint8_t kSpiMosi    = PB15;

// --- Voltage and current sensing ---
constexpr uint8_t kBatterySense    = PA0;
constexpr uint8_t kGseSense        = PB1;
constexpr uint8_t k24VCurrentSense = PB0;
constexpr uint8_t k5VSense         = PA1;
// Power enable for 3.3v current sense
constexpr uint8_t k3V3AcsPower     = PA4;
constexpr uint8_t k3V3CurrentSense = PA2;
// Power enable for 5v current sense
constexpr uint8_t k5VAcsPower      = PA6;
constexpr uint8_t k5VCurrentSense  = PA5;

// --- LEDs ---
constexpr uint8_t kDebugLed = PA7;

}

#endif // PINOUTS_H
