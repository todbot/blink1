# Makefile for "linkm-lib" and "linkm-tool"
# should work on Mac OS X, Windows, and Linux
#  
# Build arguments:
# - "OS=macosx"  -- build Mac version on Mac OS X
# - "OS=windows" -- build Windows version on Windows
# - "OS=linux"   -- build Linux version on Linux
# - "OS=wrt"     -- build OpenWrt/DD-WRT version on Linux
# - "ADDBOOTLOAD=1" -- add ability to linkm-lib and linkm-tool to use bootloader
# - "ADDBOOTLOAD=0" -- to disable ability to use bootloader
#
# Dependencies: 
# - libusb is required
# - if "make ADDBOOTLOAD=1", then
#    "../bootloadHID/commandline/linkmbootload" 
#  must be compiled beforehand if using bootload functionality
# - Or, if "make ADDBOATLOAD=0", then no other dependencies other than OS-specific ones below.
#
# Platform-specific notes:
#
# Mac OS X 
#   - Install Xcode with Unix Dev Support 
#   - Install 'make' from macports (or similar)
#
# Windows XP/7  
#   - Install MinGW and MSYS (http://www.tdragon.net/recentgcc/ )
#
# Linux (Ubuntu) 
#   - apt-get install gcc-avr avr-libc avrdude java librxtx-java
#
# OpenWrt / DD-WRT
#   - Download the OpenWrt SDK for Linux (only for Linux now, I think)
#   - set WRT_SDK_HOME environment variable
#   - type "make OS=wrt" to build
#
# BeagleBone / BeagleBoard (on Angstrom Linux)
#   - Install USB dev support 
#      "opkg install libusb-0.1-4-dev"
#   - May need to symlink libusb 
#      "cd /lib; ln -s libusb-0.1.so.4 libusb.so"
#   - Build "linkm-tool" command-line app
#      "make ADDBOOTLOAD=0"
#
#
# -----
# Based off of obdev hid-data "hidtool":
# Name: Makefile
# Project: hid-data example
# Author: Christian Starkjohann
# Creation Date: 2008-04-11
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
# This Revision: $Id: Makefile 692 2008-11-07 15:07:40Z cs $


TARGET=blink1-tool

# set to 1 to enable macosx single architecture compiling 
# (whatever your arch is, otherwise compile for x86_64, i386, and ppc )
SINGLEARCH := 0

# try to do some autodetecting
UNAME := $(shell uname -s)

ifeq "$(UNAME)" "Darwin"
	OS=macosx
endif

ifeq "$(OS)" "Windows_NT"
	OS=windows
endif

ifeq "$(UNAME)" "Linux"
	OS=linux
endif

CC=gcc


#################  Mac OS X  ##################################################
ifeq "$(OS)" "macosx"
# Mac OS X: "sudo port install libusb-legacy +universal" using MacPorts
# If you want to build for PowerPC too, you'll need to edit 
# /opt/local/etc/macports/macports.conf: universal_archs and add "ppc"
# otherwise swap the OS_CFLAGS with the commented one below
USBFLAGS = `/opt/local/bin/libusb-legacy-config --cflags`
# get just the path to the static lib
USBLIBS = `/opt/local/bin/libusb-legacy-config --libs | cut -d' ' -f1 | cut -c3- `/libusb-legacy.a
# get everything else in --libs
USBLIBS +=  `libusb-legacy-config --libs | cut -d' ' -f 3- `

ifeq ($(SINGLEARCH), 0) 
#OS_CFLAGS= -arch i386 -arch ppc 
#OS_CFLAGS= -arch i386 -arch x86_64 -arch ppc
OS_CFLAGS= -arch i386 -arch x86_64
#OS_CFLAGS= -arch x86_64
endif
##OS_CFLAGS += -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5 -no_compact_linkedit
#OS_CFLAGS += -isysroot /Developer/SDKs/MacOSX10.6.sdk -mmacosx-version-min=10.6 -no_compact_linkedit
OS_LIBS  = $(OS_CFLAGS) 
EXE_SUFFIX=
endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"
USBFLAGS=
USBLIBS=    -lhid -lsetupapi
#USBLIBS=    -lhid -lusb -lsetupapi
EXE_SUFFIX= .exe
endif

#################  Linux  ###################################################
ifeq "$(OS)" "linux"
USBFLAGS =   `libusb-config --cflags`
USBLIBS  =   `libusb-config --libs`
EXE_SUFFIX=
endif

#################  OpenWrt / DD-WRT #########################################
ifeq "$(OS)" "wrt"

WRT_SDK_HOME := $(HOME)/OpenWrt-SDK-Linux-i686-1

CC = $(WRT_SDK_HOME)/staging_dir_mipsel/bin/mipsel-linux-gcc
LD = $(WRT_SDK_HOME)/staging_dir_mipsel/bin/mipsel-linux-ld
USBFLAGS = "-I$(WRT_SDK_HOME)/staging_dir_mipsel/usr/include"
USBLIBS  = "$(WRT_SDK_HOME)/staging_dir_mipsel/usr/lib/libusb.a"

endif


#####################  Common  ##############################################

#CC=		gcc
CFLAGS = $(OS_CFLAGS) -O -Wall -std=gnu99 $(USBFLAGS) -I ../hardware/firmware 
CFLAGS += -I ./mongoose -pthread -g
LIBS=	$(OS_LIBS) $(USBLIBS) 
#LIBS += 

OBJ=		blink1-lib.o hiddata.o 

PROGRAM=	$(TARGET)$(EXE_SUFFIX)

all: msg $(PROGRAM)

msg: 
	@echo "building for OS=$(OS)"

# symbolic targets:
help:
	@echo "This Makefile works on multiple archs. Use one of the following:"
	@echo "make OS=windows ... build Windows  linkm-lib and linkm-tool" 
	@echo "make OS=linux   ... build Linux    linkm-lib and linkm-tool" 
	@echo "make OS=macosx  ... build Mac OS X linkm-lib and linkm-tool" 
	@echo "make clean ..... to delete objects and hex file"
	@echo

$(PROGRAM): $(OBJ) $(TARGET).o
	$(CC) -o $(PROGRAM) $(TARGET).o $(OBJ) $(LIBS) 

blink1-server: $(OBJ) blink1-server.o ./mongoose/mongoose.o
	$(CC) -o blink1-server $(OBJ) $(LIBS) $(CFLAGS) blink1-server.o ./mongoose/mongoose.o

.c.o:
	$(CC) $(ARCH_COMPILE) $(CFLAGS) -c $*.c -o $*.o

strip: $(PROGRAM)
	strip $(PROGRAM)

clean:
	rm -f blink1-server
	rm -f $(OBJ) $(PROGRAM)

distclean: clean
	rm -f $(TARGET).exe
	rm -f $(TARGET)

# shows shared lib usage on Mac OS X
otool:
	otool -L $(TARGET)

foo:
	@echo "OS=$(OS), USBFLAGS=$(USBFLAGS)"