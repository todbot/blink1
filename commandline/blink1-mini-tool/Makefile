#
#
#
#
#
#
# Mac OS X
#   - Install Xcode and command-line dev tools
#   - Install MacPorts
#   - sudo port install libusb-legacy
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
#
#
#

TARGET=blink1-mini-tool

#CC=gcc

# try to do some autodetecting
UNAME := $(shell uname -s)

# assume linux 
ifeq "$(OS)" ""
  OS = linux
endif

ifeq "$(UNAME)" "Darwin"
	OS = macosx
endif


#################  Mac OSx  ###################################################
ifeq "$(OS)" "macosx"
OPT_HOME := /opt/local/bin
USBFLAGS =  `$(OPT_HOME)/libusb-config --cflags`
USBLIBS  =  `$(OPT_HOME)/libusb-config --libs`
#USBFLAGS =  `$(OPT_HOME)/libusb-legacy-config --cflags`
# get just the path to the static lib
#USBLIBS  =  `$(OPT_HOME)/libusb-legacy-config --libs | cut -d' ' -f1 | cut -c3- `/libusb-legacy.a
# get everything else in --libs
#USBLIBS +=  `$(OPT_HOME)/libusb-legacy-config --libs | cut -d' ' -f 3- `

endif

#################  Linux  ###################################################
ifeq "$(OS)" "linux"
USBFLAGS =   `libusb-config --cflags`
USBLIBS  =   `libusb-config --libs`
endif

#################  OpenWrt / DD-WRT #########################################
ifeq "$(OS)" "wrt"

#WRT_SDK_HOME := $(HOME)/OpenWrt-SDK-Linux-i686-1
#CC = $(WRT_SDK_HOME)/staging_dir_mipsel/bin/mipsel-linux-gcc
#LD = $(WRT_SDK_HOME)/staging_dir_mipsel/bin/mipsel-linux-ld
#USBFLAGS = "-I$(WRT_SDK_HOME)/staging_dir_mipsel/usr/include"
#USBLIBS  = "$(WRT_SDK_HOME)/staging_dir_mipsel/usr/lib/libusb.a"

WRT_SDK_HOME := $(HOME)/projects/openwrt/sdk/OpenWrt-SDK-ar71xx-for-Linux-i686-gcc-4.3.3+cs_uClibc-0.9.30.1
CC = $(WRT_SDK_HOME)/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/usr/bin/mips-openwrt-linux-gcc
LD = $(WRT_SDK_HOME)/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/usr/bin/mips-openwrt-linux-ld
USBFLAGS = "-I$(WRT_SDK_HOME)/staging_dir/target-mips_r2_uClibc-0.9.30.1/usr/include"
USBLIBS  = "$(WRT_SDK_HOME)/staging_dir/target-mips_r2_uClibc-0.9.30.1/usr/lib/libusb.a"
LDFLAGS += -static
endif


CFLAGS=	$(OS_CFLAGS) -O -Wall -std=gnu99 $(USBFLAGS)
LIBS=	$(OS_LIBS) $(USBLIBS) $(LDFLAGS)

OBJ=		$(TARGET).o hiddata.o 

PROGRAM=	$(TARGET)$(EXE_SUFFIX)

all: msg $(PROGRAM)

msg: 
	@echo "building for OS=$(OS)"

# symbolic targets:
help:
	@echo "This Makefile works on multiple archs. Use one of the following:"
	@echo "make OS=linux   ... build for Linux"
	@echo "make OS=macosx  ... build for Mac OS X "
	@echo "make clean ..... to delete objects and hex file"
	@echo

$(PROGRAM): $(OBJ)
	$(CC) -o $(PROGRAM) $(OBJ) $(LIBS) 


strip: $(PROGRAM)
	strip $(PROGRAM)

clean:
	rm -f $(OBJ) $(PROGRAM)

.c.o:
	$(CC) $(ARCH_COMPILE) $(CFLAGS) -c $*.c -o $*.o

# shows shared lib usage on Mac OS X
otool:
	otool -L $(TARGET)

foo:
	@echo "OS=$(OS), USBFLAGS=$(USBFLAGS)"
