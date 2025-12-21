# The GUI

This folder contains the graphical user interface application for controlling the radio device.

## Working on the project

This project is designed to use Qt 6.10.1 together with several development tools it provides. To install them, visit [this page](https://www.qt.io/development/download-qt-installer-oss) and choose the right installer for your host system.

From the installer, you should select the following individual components:
- Qt -> Qt 6.10.1 -> MinGW 13.1.0 64-bit
- Qt -> Qt 6.10.1 -> Qt Debug Information Files
- Qt -> Build Tools -> MinGW 13.1.0 64-bit

The Qt Core extension configures the CMake kit properly, so we use that for building. We do not need CMake or Ninja from the Qt installer as we utilize the "cube-cmake" executable provided by the STM32 extensions. If you wish to develop this project in isolation, without the STM32 extensions or bundles it provides you will need to manually install CMake and Ninja, and ensure they are found from the PATH environment variable when VS Code is started.

## Debugging

The `launch.json` file has the necessary wirings for debugging both the C++ and the QML portions. It uses the Qt C++ extension's commands to set things up properly.
