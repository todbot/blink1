Command-line Tool and C library for blink(1)
============================================

To build, see the Makefile.

The current support build products are:

- `blink1-tool` -- command-line tool for controlling blink(1)
- `blink1-lib` -- C library for controlling blink(1)


Supported platforms:

- Mac OS X 10.6.8, 10.7+
- Windows XP+ (built with MinGW & MSYS)
- Linux (most all, primary development on Ubuntu)
- FreeBSD 8.2+
- Raspberry Pi (Raspian)
- BeagleBone (Ubuntu)
- OpenWRT / DD-WRT
- ... just about anything else with Gnu Make & a C-compiler

In general, the `blink1-tool` builds as a static binary where possible,
eliminating the need for shared library dependencies on the target.
However, static builds can be problematic for some systems with "different" 
libusb implementations, so doing `make EXEFLAGS=` will generally build a non-static version.





