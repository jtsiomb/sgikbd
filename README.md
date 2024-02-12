SGI - PS/2 keyboard and mouse converter
=======================================

About
-----
This project is a converter for connecting PS/2 keyboards and mice to old
Silicon Graphics computers which had a proprietary keyboard/mouse interface. It
should work on every non-PS/2 SGI computer with mini-DIN6, DB9 and DB15
keyboard/mouse connectors, such as:

  - Iris 4D series
  - Onyx
  - Indigo

This converter is not necessary, and will not work, for any later SGI machines,
like the indy, O2, indigo2, Octane, and so on, because they use PS/2 keyboards
and mice directly.

> Important note for R3000 indigo owners, a simple hack is necessary to make
> sgikbd work with R3000 indigos.
> See http://nuclear.mutantstargoat.com/hw/sgikbd/indigo_r3000_hack.html

It's based on an AVR atmega328pb microcontroller, which reads the PS/2
keyboard and mouse ports, and translates scancodes and events to the protocol
and electrical signalling used by the SGI computers. For details on the
SGI signalling and protocols, see the copy of the keyboard and mouse manpages
from the section 7 of the IRIX manual, under `doc/`.

Directory structure:

  - `hw` - hardware: kicad files and pdf schematics.
  - `fw` - firmware for the AVR microcontroller.
  - `case` - 3D printable enclosure for the sgikbd board.
  - `doc` - relevant documentation.

This repository only contains source files. For schematics in PDF format,
zipped gerbers for PCB manufacturing, and human-readable bill of materials, as
well as detailed build instructions, visit the project website:
http://nuclear.mutantstargoat.com/hw/sgikbd

Alternatively download the latest release archive from:
https://github.com/jtsiomb/sgikbd/releases

The firmware is based on my previous
[a500kbd](https://github.com/jtsiomb/a500kbd) project, so if you see any
leftover references to the Amiga anywhere don't be confused.

License
-------
Copyright (C) 2017-2022 John Tsiombikas <nuclear@mutantstargoat.com>

You are free to use, reproduce, modify, and redistribute any part of this
project, provided you make any derivative work you release, freely available
under the same terms.

Both hardware designs, and software parts of this project, are released under
the terms of the GNU General Public License v3, or at your option any later
version published by the Free Software Foundation. See `LICENSE` for details.

To comply with the license, when releasing derivative hardware designs, you must
provide the full EDA project files, and of course firmware/software source code.
Releasing only images of schematics, gerbers, and firmware binaries is *not*
sufficient for compliance.
