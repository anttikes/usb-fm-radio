# USB FM Radio
This project is a USB FM Radio device based on an STM32 microcontroller and the Si4705 FM/RDS tuner chip. It includes the schematic and firmware portions in separate folders. This file prodides context for AI coding agents to be productive in this repository.

## Firmware
The firmware is the code that is executed by the STM32F042C6T6 microcontroller. Its main functionality is to control the Si4705 tuner chip, handle USB communication, and manage audio streaming. The USB portion is delegated to the TinyUSB stack, whose code files live in the "Firmware/ThirdParty/tinyusb/" directory. The project uses STM32 HAL libraries.

The firmware provides instructions to and reads responses from Si4705 over I2C, and streams audio data via I2S to the connected USB host.

## Schematic
The schematic folder contains KiCad files that define the hardware design and PCB layout of the device. Files in this folder should not be used by the agent.

## How to build the project
- Inspect available presets: `cmake --list-presets`
- Configure Debug build (PowerShell):
  - `cmake --preset Debug`
  - `cmake --build build/Debug`
- For Release: `cmake --preset Release` then `cmake --build build/Release`.
- `compile_commands.json` is generated in `build/` — use it to provide accurate include paths for static analysis.

## How to debug and flash the firmware
The agent is not required to operate the debugging and flashing tools, but it should be aware of their existence in case it needs to modify related code or scripts.

# Code patterns and conventions
- Third-party code: do not edit files in `Firmware/ThirdParty/`.
- Generated code: do not edit files in `Firmware/Generated/` — regenerate from CubeMX when structural changes are needed.
- Firmware main module lives in `Firmware/Core/`, and includes peripheral initialization, low-level interrupt handlers, clock setup, main application loop and HAL configuration and initialization code
- Firmware radio logic lives in `Firmware/Radio/`, and includes common definitions for the Si4705 tuner, defines commands and responses, provides a structure to represent the tuner's state
- Firmware USB logic lives in `Firmware/USB/`, and includes TinyUSB integration code, USB descriptors, and TinyUSB callback functions.

**Do / Don't**
- Do: prefer small, focused changes and run a local build with the correct preset after the changes to ensure they still compile.
- Don't: edit `Firmware/Generated/`.
- Don't: edit `Firmware/ThirdParty/`.
- Don't: make commits or add files to git staging

If the agent is unsure about what it should do, it should ask for clarification using yes/no questions.
