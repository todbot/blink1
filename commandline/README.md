Command-line Tool and C library for blink(1)
============================================

To build, see the Makefile.

The current support build products are:

- `blink1-tool` -- command-line tool for controlling blink(1)
- `blink1-lib` -- C library for controlling blink(1)
- `blink1-tiny-server` -- Simple HTTP API server to control blink1, uses blink1-lib

Type `make help` for a full list.

Also see in this directory:
- `blink1control-tool` -- blink1-tool for use with Blink1Control (uses HTTP REST API)
- `blink1-mini-tool` -- commandline tool using libusb-0.1 and minimal deps
- `blink1raw` -- example commandline tool using Linux hidraw
- `scripts` -- examples scripts using blink1-tool

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





