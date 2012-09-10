# Makefile for "blink1-lib" and "blink1-tool"
# should work on Mac OS X, Windows, Linux, and other Linux-like systems
#  
# Build arguments:
# - "OS=macosx"  -- build Mac version on Mac OS X
# - "OS=windows" -- build Windows version on Windows
# - "OS=linux"   -- build Linux version on Linux
# - "OS=wrt"     -- build OpenWrt/DD-WRT version on Linux
#
# Dependencies: 
# - libusb is required
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
#   - apt-get install gcc-avr avr-libc 
#   - apt-get install libusb-1.0
#
#   optionally, to build 32-bit on 64-bit Ubuntu, try a chrooted build:
#   (warning this will use up a lot of disk space)
#   - sudo apt-get install ubuntu-dev-tools
#   - pbuilder-dist oneiric i386 create
#   - mkdir $HOME/i386
#   - cp -r blink1 $HOME/i386
#   - pbuilder-dist oneiric i386 login --bindmounts $HOME/i386
#     (now in the chrooted area)
#   - apt-get install libusb-1.0-0 libusb-1.0-0-dev
#   - cd $HOME/i386/blink1
#   - CFLAGS='-I/usr/include/libusb-1.0' LIBS='-lusb-1.0' make
#   - exit
#   
# BeagleBone / BeagleBoard (on Angstrom Linux)
#   - opkg install libusb-0.1-4-dev  (FIXME: uses HIDAPI & libusb-1.0 now)	
#   - May need to symlink libusb 
#      cd /lib; ln -s libusb-0.1.so.4 libusb.so
#   - make
#
# OpenWrt / DD-WRT
#   - Download the OpenWrt SDK for Linux (only for Linux now, I think)
#   - set WRT_SDK_HOME environment variable
#   - type "make OS=wrt" to build
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

CFLAGS += -arch i386 -arch x86_64
CFLAGS += -pthread
LIBS += -framework IOKit -framework CoreFoundation

OBJS = ./hidapi/mac/hid.o
EXE=
endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"
#CFLAGS += 
#LIBS +=  -mwindows -lsetupapi -Wl,--enable-auto-import -static-libgcc -static-libstdc++ -lkernel32 
LIBS +=  -mwindows -lsetupapi -Wl,-Bdynamic -lgdi32 -Wl,--enable-auto-import -static-libgcc -static-libstdc++ -lkernel32
OBJS = ./hidapi/windows/hid.o
EXE= .exe
endif

#################  Linux  ###################################################
ifeq "$(OS)" "linux"
#USBFLAGS =   `libusb-config --cflags`
#USBLIBS  =   `libusb-config --libs`
#LIBS   += `pkg-config libudev --libs` -lrt
#CFLAGS += `pkg-config libusb-1.0 --cflags`
#CFLAGS += -m32
CFLAGS += `pkg-config libusb-1.0 --cflags`
LIBS   += `pkg-config libusb-1.0 --libs` -lrt -lpthread -ldl -static
OBJS = ./hidapi/libusb/hid.o
EXE=
endif


#####################  Common  ##############################################

CC = gcc

CFLAGS += -std=gnu99 -I ../hardware/firmware 
#CFLAGS += -O -Wall -std=gnu99 -I ../hardware/firmware 
CFLAGS += -I./hidapi/hidapi -I./mongoose -g
# only uncomment for debugging HIDAPI stuff
#CFLAGS += -DDEBUG_PRINTF

OBJS +=  blink1-lib.o 


#all: msg blink1-tool blink1-server-simple
all: msg blink1-tool 

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

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

blink1-tool: $(OBJS) blink1-tool.o
	$(CC) $(CFLAGS) -c blink1-tool.c -o blink1-tool.o
	$(CC) -g $(OBJS) $(LIBS) blink1-tool.o -o blink1-tool$(EXE) 

blink1-server-simple: $(OBJS) blink1-server-simple.c
	$(CC) $(CFLAGS) -c blink1-server-simple.c -o blink1-server-simple.o
	$(CC) $(CFLAGS) -c ./mongoose/mongoose.c -o ./mongoose/mongoose.o
	$(CC) -g $(OBJS) ./mongoose/mongoose.o $(LIBS) blink1-server-simple.o -o blink1-server-simple$(EXE)


clean: 
	rm -f $(OBJS) 
	rm -f blink1-server-simple.o blink1-tool.o
	rm -f mongoose/mongoose.o

distclean: clean
	rm -f blink1-tool$(EXE)
	rm -f blink1-server-simple$(EXE)

# shows shared lib usage on Mac OS X
otool:
	otool -L $(TARGET)

foo:
	@echo "OS=$(OS), USBFLAGS=$(USBFLAGS)"