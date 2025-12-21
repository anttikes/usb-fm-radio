# The GUI

This folder contains the graphical user interface application for controlling the radio device.

## Working on the project

This project is designed to use Qt 6.10.1 together with several development tools it provides. To install them, visit [this page](https://www.qt.io/development/download-qt-installer-oss) and choose the right installer for your host system.

From the installer, you should select the following individual components:
- Qt -> Qt 6.10.1 -> MinGW 13.1.0 64-bit
- Qt -> Qt 6.10.1 -> Additional Libraries -> Qt Language Server
- Qt -> Qt 6.10.1 -> Additional Libraries -> Qt Serial Bus
- Qt -> Build Tools -> MinGW 13.1.0 64-bit
- Qt -> Build Tools -> CMake 3.30.5
- Qt -> Build Tools -> Ninja 1.12.1

After installation ensure that `CMakePresets.json` configures the `Qt6_DIR` environment variable correctly. It must point to the root folder where you installed Qt. Adjust `Qt6_VERSION` if you wish to deviate from 6.10.1.


## Deployment and debugging

The `launch.json` file has the necessary wirings for debugging the application. It uses the Qt Core extension's `qt-core.qtInstallationRoot` to find the installation directory. Similar approach is not possible in `CMakePresets.json` because the preset file is evaluated by CMake and thus no macros which apply to VS Code are evaluated.
