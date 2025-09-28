# The firmware

This folder contains the firmware application for the STM32 microcontroller.

## Working on the project

To build this project you will need the STM32CubeIDE from ST Microelectronics, and the STM32F0Cube firmware package for the IDE (for code generation purposes). Using CubeIDE requires you to register an account with ST Microelectronics, as otherwise you will not be able to download the firmware package.

At a later stage this repository will migrated to VS Code and cleaner sub-module usage.

## Deployment and debugging

To deploy and debug the project you will need a fabricated board and the ST Microelectronics' ST-Link v3 debug probe. The current schematic has a header for plugging in the probe.
