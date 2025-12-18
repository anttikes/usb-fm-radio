# The GUI

This folder contains the graphical user interface application for controlling the radio device.

## Working on the project

To build this project you will need the Qt 6.10 or later development tools installed. To install them, visit [this page](https://www.qt.io/development/download-qt-installer-oss) and choose the right installer for your host system.

From the installer, you should select the following individual components. The version numbers are for guidance only, you can also choose newer components:
- Qt -> Qt 6.10.1 -> MinGW 13.1.0 64-bit
- Qt -> Qt 6.10.1 -> Additional Libraries -> Qt Language Server
- Qt -> Qt 6.10.1 -> Additional Libraries -> Qt Serial Bus
- Qt -> Build Tools -> MinGW 13.1.0 64-bit (or newer)
- Qt -> Build Tools -> CMake 3.30.5 (or newer)
- Qt -> Build Tools -> Ninja 1.12.1 (or newer)

## Deployment and debugging

The `launch.json` file has the necessary wirings for debugging the application.
