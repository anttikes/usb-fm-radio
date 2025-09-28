# USB FM Radio

## Overview

This intent of this repository is to present a fully working USB FM Radio hardware device, consisting of the electronic schematic and PCB layout files drawn with KiCad, a C-language firmware implementation for the microcontroller and a GUI application for operating the radio device from Windows and Linux.

> [!IMPORTANT]
> This repository is still under heavy development, and quite many of the things are still unfinished.

Demonstration highlights:
 - The schematic consists of an STM32F042-based microcontroller, Si4705-D60 FM Radio Receiver, SG-3030CM crystal oscillator, a radio frontend circuit and the support circuitry
 - The microcontroller controls the radio chip via an Inter-Integrated Circuit (I2C) interface, and receives audio from the tuner via an Inter-Integrated Circuit Sound (I2S) interface
 - The firmware presents a USB 2.0 -compliant USB Audio device for the host computer, together with a USB 2.0 Human Interface Device (HID) device for controlling the tuner chip
 - The GUI application grants the user the ability to control the hardware device, tune to a station, adjust volume, and other operations

## Copyright and License

This repository consists of several independent sections, some delivered by third parties, and some by the author of the repository. Therefore it falls under several copyrights and licenses.

The original work presented in this repository (the  KiCad project & files, portions of firmware not originating from a third party and the GUI application) are copyright of the author. Due to the "work in progress" nature of this repository some files may have a copyright statement while others do not. In case a file does not contain a copyright statement, and clearly does not belong to a third party then it is part of the original work, and therefore copyright of the author.

Similarly, the original work is entirely licensed under the Creative Commons NonCommercial license (CC BY-NC).

This means you are free to use the original work for your own use, for education, entertainment, or perhaps for building a copy of the device for yourself. However, any commerical use whatsoever is strictly prohibited. The demonstration project's intent is to showcase the skills of the author, and not to provide you with a ready-made product that you can sell for profit.

## How to use

This repository has been created using the `git submodule` command:
1. To **clone** this repository along with the linked submodules, option `--recursive` has to be specified as shown below.

```bash
git clone --recursive https://github.com/anttikes/usb-fm-radio.git
```

2. To get the **latest updates**, in case this repository is already on your local machine, issue the following commands.

```bash
git pull
git submodule update --init --recursive
```
