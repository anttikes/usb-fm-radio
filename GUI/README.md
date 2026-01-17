# The GUI

This folder contains the graphical user interface application for controlling the radio device.

![Main Screen](docs/MainScreen.png)

## Working on the project

This project uses Qt 6.10.1 together with several development tools provided by the installer. Begin by visiting [this page](https://www.qt.io/development/download-qt-installer-oss) and choose the right Qt Maintenance Tool installer for your system.

From the installer, you should select the following individual components:
- Qt -> Qt 6.10.1 -> MinGW 13.1.0 64-bit
- Qt -> Qt 6.10.1 -> Qt Debug Information Files
- Qt -> Build Tools -> MinGW 13.1.0 64-bit
- Qt -> Build Tools -> CMake 3.30.5
- Qt -> Build Tools -> Ninja 1.12.1

After installing, you must add the CMake and Ninja binary folders to your user-specific PATH variable, and restart VS Code. The Qt Core extension should configure the CMake kit when it is first started and once configure you should be able to select the "GUI" folder from the CMake pane in VS Code, and build it with "Build" from the VS Code's status bar.

## Debugging

The `launch.json` file has the necessary wirings for debugging both the C++ and the QML portions. It uses the Qt C++ extension's commands to set the debugger and symbol file paths for Qt. Starting the debug session takes a long while since GDB likes to load symbol files of Windows' shared DLLs. Using `set auto-solib-add off` will not work, probably due to a bug in VS Code.

## Acknowledgements

This repository uses fonts from the DSEG7 series font family. Copyright, license terms and other information related to this font can be found from [keshikan/DSEG](https://github.com/keshikan/DSEG).