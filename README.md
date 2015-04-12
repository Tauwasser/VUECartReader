# USB interface/dumper for the Nintendo® Virtual Boy™ Cartridges

This is the software for the Virtual Boy™ (VUE) Cart Reader. The reader consists of an ATmega32U2 which interfaces
with USB and a VUE Game Edge Connector (GEC) Receptacle (REC) to communicate both ways.

For information on the hardware, check out the hardware branch *reader_hw*.

For information of the firmware, check out the firmware branch *master*.

## Features

Write to and read from ROM only at this moment. ATmega32U2 is a USB 2.0 FS device.
Measuring actual speed, the VUE Cart Reader was able to transfer 208kByte/s (1.6 Mbit/s). Transferring 
1 MByte takes roughly 8 seconds.

Code could probably be optimized further. Writing/Reading other areas planned and can be trivially achieved by editing source code right now.

## Compilation

At the moment, I don't have a Makefile ready. Compile everything with your favorite C++ compiler. Then link it. Then use it.

Dependencies:

- Boost.ProgramOptions
- Boost.FileSystem
- Boost.System
- Boost.Locale
- libusb

## Licensing

This software is licensed under the GNU General Public License v3.

```
VUE Cart Reader Microcontroller Software
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

Other software, e.g. software dependencies are licensed under the terms of their respective licenses.

## Legalese

Virtual Boy™ is a trademark by Nintendo. Nintendo® is a registered trademark.