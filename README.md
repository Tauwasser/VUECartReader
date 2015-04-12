# USB interface/dumper for the Nintendo® Virtual Boy™ Cartridges

This is the hardware design for the Virtual Boy™ (VUE) Cart Reader. The reader consists of an ATmega32U2 which interfaces
with USB and a VUE Game Edge Connector (GEC) Receptacle (REC) to communicate both ways.

For information on the firmware, check out the firmware branch *master*.

For information of the software, check out the software branch *reader_sw*.

## Principle of Operation

The ATmega32U2 serves as USB 2.0 FS interface. It's clocked at 16 MHz.

The most important GEC control signals are directly connected to the ATmega32U2.

To extend the I/Os, the design uses a 16-bit wide address/data multiplex bus. The address lines as well as 
non-critical control and status lines are driven by four 8-bit DFFs.

The general idea is:

1. Deassert all GEC control signals, i.e. cartridge does not drive AD bus.
2. Drive lower address bits on AD bus.
3. Latch into the first set of two 8-bit DFFs.
4. Drive upper address/control/status bits on AD bus.
5. Latch into the second set of two 8-bit DFFs.
6. Read: Stop driving AD bus and use internal pull-ups.
Write: Drive 16-bit data word on AD bus.
7. Assert appropriate control lines.
8. Deassert control lines, AD bus remains pulled up.

Loading the full 23-bit address on every word would take some time. Therefore, an external Texas
Instruments Little Logic chip (74LVC1G98DW-7) implements a function that allows loading only the lower
15 address bits, while the upper 7 address bits as well as the status/non-critical control bits
remain the same. This relies on component delay, so it might not work for all components at
various temperatures, but is a nice shortcut.

The 74LVC1G98DW-7 can be replaced by 74LVC2G14 to always load both upper and lower address bits and 
forgo the advantage mentioned above. If the 74LVC1G98DW-7 is fitted and parallel loading the DFFs does
not work as intended, cut CL1, close SL1 to have the same functionality as 74LVC2G14.

There is one green power LED and one red status LED. There is an optional cartridge audio IDC header.
Optionally, #INTCRO can be conncted to #RESET when configured as GPIO.

VUE Cart Reader v1.1 has the following bug: PD7 (#HWB) which is used for AD7 is left floating at reset.
The board will sporadically boot up into the bootloader instead of the firmware. To fix this, clear
the ATmega32U2 HWB fuse.

VUE Cart Reader v1.2 has an optional jumper header and PD7 (#HWB) pulled high by default. Use a jumper
to boot into the bootloader.

## CAD and Manufacture

CAD files are Altium Designer files. Project outputs are:

- Cart_Reader_V1.1.zip: Gerber and drill files used to manufacture VUE Cart Reader v1.1
- Cart_Reader_V1.2.zip: Gerber and drill files for manufacturing VUE Cart Reader v1.2 (never done)
- CartReader_v1.2.PDF: PDF file with
  * logical schematic,
  * minimal physical schematic,
  * top and bottom layer printouts,
  * multilayer printouts,
  * bill of materials

Both, v1.1 and v1.2 require some additional drilling around the mounting holes of the VUE-GEC-REC, because
my footprint is not 100% correct. v1.2 is untested as of now.

L1 and L2 are somewhat optional. I strongly urge to fit both. If not fitted, bridge using solder.

Most GEC resistors are just there for the case when a game is inserted while turning the VUE Cart Reader on.
 #ROM_CE and #RAM_CS pull-up are kind of pointless now that the DFF drives them all the time anyway.

No commercial cartridge uses the audio mixing, so the header P2 need not be fitted. Header P5 should only
be needed once and might be circumvented using ISP. R14 is there in case header P5 is fitted
and jumper is left in during operation.


## Licensing

This hardware design is licensed under the Creative Commons Attribution-ShareAlike 4.0
International License.

```
Copyright (C) 2015 Tauwasser (tauwasser@tauwasser.eu)

This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
International License.

To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/
or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
```

## Legalese

I'm not affiliated with Nintendo in any way.

Virtual Boy™ is a trademark by Nintendo. Nintendo® is a registered trademark. All other trademarks are property of their respective owner.
