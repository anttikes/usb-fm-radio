# USB FM Radio - Firmware

> [!IMPORTANT]
> This repository uses Git submodules; use `git clone <url>` when cloning it, and then `.\Initialize-Submodules.ps1` to init the submodules
> Using recursive submodule init during clone will download quite a lot of unnecessary stuff.

> [!IMPORTANT]
> This repository is still under heavy development, and quite many of the things are still unfinished.

## Overview

This repository provides the firmware portion of the USB FM Radio project. It presents a USB 2.0 -compliant USB Audio 1.0 device for the host computer, together with a USB Human Interface Device (HID) 1.11 device for controlling the tuner chip.

The USB portions are built on top of the `tinyusb` project.

## Copyright and License

The original work presented in this repository (the KiCad project & files, portions of firmware not originating from a third party, and the GUI application) are copyrighted to the owner of this repository. These portions are entirely licensed under the Creative Commons NonCommercial Sharealike license (CC-BY-NC-SA).

The implications of this license can be found in https://creativecommons.org/licenses/by-nc-sa/4.0/
