# USB FM Radio

## Overview

This intent of this repository is to present a fully working USB FM Radio hardware device, consisting of the electronic schematic and PCB layout files drawn with KiCad, a C-language firmware implementation for the microcontroller and a GUI application for operating the radio device from Windows and Linux.

> [!IMPORTANT]
> This repository is still under heavy development, and quite many of the things are still unfinished.

Demonstration highlights:
 - The schematic consists of an STM32F042-based microcontroller, Si4705-D60 FM Radio Receiver, SG-3030CM crystal oscillator, a radio frontend and the supporting circuitry
 - The microcontroller controls the radio chip via an I2C interface, and receives audio from the tuner via an I2S interface
 - The firmware presents a USB 2.0 -compliant USB Audio device for the host computer, together with a Human Interface Device (HID) device for controlling the tuner chip
 - The GUI application allows the user to control the device, tune to a station, adjust volume, and other operations

## Copyright and License

This repository consists of both original work by the author, and from code provided by third parties. Therefore it falls under several copyrights and licenses.

The original work presented in this repository (the KiCad project & files, portions of firmware not originating from a third party, and the GUI application) are copyrighted to the owner of this repository. These portions are entirely licensed under the Creative Commons NonCommercial Sharealike license (CC-BY-NC-SA).

The implications of this license can be found in https://creativecommons.org/licenses/by-nc-sa/4.0/
