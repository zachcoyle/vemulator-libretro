VeMUlator
=========

This is a port of the Android SEGA Dreamcast VMU emulator "VeMUlator" for libretro, it was translated from Java to C++ and then implemented the libretro.h callbacks.

## Known issues:
* Timer problems (Mainly T0, due to lack of documentation of the VMU.)
* Sound not being synchronized with the system.


## To be add:
* Support for playing libretro save files (.srm) with write capabilities (To be compatible with libretro's Dreamcast cores.)
* BIOS support.
