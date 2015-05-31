# USB interface/dumper for Nintendo® Virtual Boy™ Cartridges

This is the firmware for the Virtual Boy™ (VUE) Cart Reader. The reader consists of an ATmega32U2 which interfaces
with USB and a VUE Game Edge Connector (GEC) Receptacle (REC) to communicate both ways.

For information on the hardware, check out the hardware branch *reader_hw*.

For information of the software, check out the software branch *reader_sw*.

## Features

Write to and read from ROM, RAM or expansion areas of your cartridge. ATmega32U2 is a USB 2.0 FS device.
Measuring actual speed, the VUE Cart Reader was able to transfer 208kByte/s (1.6 Mbit/s). Transferring 
1 MByte takes roughly 8 seconds.

Code could probably be optimized further to allow for prefetching or posted writes.

## Compilation

Use Atmel Studio v6.1 or later to compile this firmware. You should have at least one .hex file after 
successful compilation.

## Initial Programming

You will need an In-System Programmer (ISP) for initial programming. 

1. Flash fuses to disable CLKDIV8 and make sure the bootloader size is right for you.
2. Use default bootloader or flash your own bootloader, ex. I use Dean Camera's DFU bootloader.
3. You should be able to 

1. Flash fuses to disable CLKDIV8.
2. VUE Cart Reader v1.1 only: Disable HWBE fuse as well, or device will spuriously boot into booloader.
3. Make sure the fuses are right for your bootloader. I use Dean Camera's LUFA DFU bootloader, which sits at 0x3800 (word address).
4. VUE Cart Reader v1.1 only: Program BOOTRST fuse to make sure you boot into the bootloader.
5. You should be able to use DFU now. See below.
6. VUE Cart Reader v1.1 only: After flashing this firmware for the first time, disable BOOTRST.

## Firmware Flashing

To flash this firmware, use Device Firmware Update (DFU) over USB, e.g. Atmel FLIP.

## Licensing

This firmware is licensed under the GNU General Public License v3.

```
VUE Cart Reader Microcontroller Firmware
Copyright (C) 2015 Tauwasser (tauwasser@tauwasser.eu)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```

Other software, e.g. Atmel Software Framework and the USB library LUFA are licensed under the terms
of their respective licenses. These parts of software are only included because I am unaware how not
to include them and avoid version conflicts.

## Legalese

I'm not affiliated with Nintendo in any way.

Virtual Boy™ is a trademark by Nintendo. Nintendo® is a registered trademark. All other trademarks are property of their respective owner.

USB VID/PID are LUFA Test VID/PID. This is a one-off development project and not a commercial product. Public hosting of thise code does not constitute a release to the general public.