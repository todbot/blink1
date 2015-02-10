Making Releases of blink1-tool
==============================

[this is mostly notes to Tod and other maintainers on how to do new releases]

Steps when a new release of blink1-tool (and blink1-lib) is to be made

Assumptions
------------

- Target platforms are primarily:
  - Mac OS X 10.8+
  - Windows XP+
  - Linux 32-bit - Ubuntu 14
  - Raspberry Pi
  - Arduino Yun / OpenWrt ar71xx
  - OpenWrt brcm47xx


- Build primarily 32-bit, not 64-bit
- Unix command-line build tools available for each platform
- Shared mounted checkout of github.com/todbot/ blink1 repo,
   or at least same revision checkout

General Process
---------------

1. `cd  blink1/commmandline`
2. Build code with `make clean && make`
3. Package up zipfile with `make package`
4. Copy zip package to and test on separate test systems
5. Publish zip package to github release
6. Adjust links on http://blink1.thingm.com/downloads


Example
-------
```
% cd blink1/commandline
% make clean && make
% make package
```

Platform Specifics (for Tod mostly)
-----------------------------------

### Mac

Environment: Build on 10.10 on MacBookPro Retina, test on 10.8.2 Mac Mini

Command: `make clean && make && make package`

### Windows

Build on Win7-64bit, test on Win7-32 bit, Win8-64bit (all in VMs)

Command: `make clean && make && make package`

### Linux - Ubuntu

Build on Ubuntu 14 VM, test on Ubuntu 12 VM

Command: `make clean && make && make package`

### Linux - Raspberry Pi

Environemt: Build on Raspberry Pi running Raspbian.

Command: `git pull && make distclean && make && make package`

Rename zip packages from "armv6l" to "raspi".

### OpenWrt - Yun / ar71xx

On Fedora 14 VM using OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2

Command: `make clean && make OS=yun && make package`

### OpenWrt - brcm71xx
On Fedora 14 VM using OpenWrt-SDK-brcm47xx-for-linux-i486-gcc-4.6-linaro_uClibc-0.9.33.2

Command: `make clean && make OS=wrt && make package`



