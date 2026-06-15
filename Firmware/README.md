# Power Module Firmware

This directory contains the firmware for the avionics Power Modules. The module firmware in this directory should be mostly functional.

We currently use the Arduino IDE with the STM32Duino core ("STM32 MCU based boards" by STMicroelectronics, working with 2.7.1) for developing firmware for our modules. We flash firmware to the modules using SWD via an STLinkV2.

## Attributions

Our firmware uses a variety of external libraries. Most libraries can be found through the Arduino library manager, and those that cannot have the corresponding GitHub link beside their include statements. We have taken care not to include any external libraries within our repository; only code we have written (or have modified and credited) are included here.
