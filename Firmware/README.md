# The firmware

This folder contains the firmware application for the STM32 microcontroller.

## Working on the project

To build this project you will need Visual Studio Code and the STM32Cube extensions from ST Microelectronics. Once you open the multi-root workspace file from the root directory, VS Code should recommend these extensions for you.

After the extensions are installed they should prompt you for downloading and installing the following bundles that have been configured in the `.settings\bundles.store.json` file:
 - CMake
 - Ninja
 - GNU Tools for STM32
 - ST ARM clangd
 - STLink GDB Server
 - ST MCU Programmer

During the installation of the extensions and the bundles you may need to restart Visual Studio Code. Once everything is installed open the CMake pane in VS Code, select the "Firmware" folder and hit the "Build" button from the VS Code's status bar.

## Deployment and debugging

To deploy and debug the project you will need a fabricated board and the ST Microelectronics' ST-Link v3 debug probe. The current schematic has a header for plugging in the probe.
