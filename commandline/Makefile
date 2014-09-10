# Makefile for "blink1-lib" and "blink1-tool"
#
# Works on Mac OS X, Windows, Linux, and other Linux-like systems.
# Type "make help" to see supported platforms.
#  
# Build arguments:
# - "OS=macosx"  -- build Mac version on Mac OS X
# - "OS=windows" -- build Windows version on Windows
# - "OS=linux"   -- build Linux version on Linux
# 
# Architecture is usually detected automatically, so normally just type "make".
#
# Dependencies: 
# - hidapi (included), which uses libusb on Linux-like OSes
#
# Platform-specific notes:
#
# Mac OS X 
#   - Install Xcode with "Unix Dev Support" and "Command-line tools" (in Preferences)
#   - make
#
# Windows XP/7  
#   - Install MinGW and MSYS (http://www.tdragon.net/recentgcc/ )
#   - make
#
# Linux (Ubuntu) 
#   - apt-get install build-essential pkg-config libusb-1.0-0-dev
#   - make
#
# Linux (Fedora 18+)
#   - yum install make gcc 
#   - make 
#
# Linux (Fedora 17)
#   - yum install make gcc libusb1-static glibc-static
#   - make
#
# FreeBSD
#   - libusb is part of the OS so no pkg-config needed.
#   - No -ldl on FreeBSD necessary.
#   - For FreeBSD versions < 10, iconv is a package that needs to be installed;
#     in this case it lives in /usr/local/lib/
#   - On FreeBSD 8.3, this command builds blink1-tool: 
#   - "cd blink1/commandline && USBLIB_TYPE=HIDDATA gmake"
#
# Linux Ubuntu 32-bit cross-compile on 64-bit
#   To build 32-bit on 64-bit Ubuntu, try a chrooted build:
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
# Raspberry Pi
#   - apt-get install libusb-1.0.0-dev
#   - make
#
# BeagleBone / BeagleBoard (on Angstrom Linux)
#   - opkg install libusb-0.1-4-dev  (FIXME: uses HIDAPI & libusb-1.0 now)	
#   - May need to symlink libusb 
#      cd /lib; ln -s libusb-0.1.so.4 libusb.so
#   - make
#
#


# pick low-level implemenation style
# "HIDAPI" type is best for Mac, Windows, Linux Desktop, 
#  but has dependencies on iconv, libusb-1.0, pthread, dl
#
# "HIDDATA" type is best for low-resource Linux, 
#  and the only dependencies it has is libusb-0.1
#
# Try either on the commandline with "make USBLIB_TYPE=HIDDATA" 
#

USBLIB_TYPE ?= HIDAPI
#USBLIB_TYPE = HIDDATA

# uncomment for debugging HID stuff
#CFLAGS += -DDEBUG_PRINTF


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

ifeq "$(UNAME)" "FreeBSD"
	OS=freebsd
endif


GIT_TAG="$(strip $(shell git tag | tail -1))"
MACH_TYPE="$(strip $(shell uname -m))"

BLINK1_VERSION="$(GIT_TAG)-$(OS)-$(MACH_TYPE)"




#################  Mac OS X  ##################################################
ifeq "$(OS)" "macosx"
LIBTARGET = libBlink1.dylib
CFLAGS += -mmacosx-version-min=10.6 
#CFLAGS += -fsanitize=address

ifeq "$(USBLIB_TYPE)" "HIDAPI"
CFLAGS += -DUSE_HIDAPI
CFLAGS += -arch i386 -arch x86_64
# don't need pthread with clang
#CFLAGS += -pthread
CFLAGS += -O2 -D_THREAD_SAFE -MT MD -MP 
CFLAGS += -I./hidapi/hidapi 
OBJS = ./hidapi/mac/hid.o
endif

ifeq "$(USBLIB_TYPE)" "HIDDATA"
CFLAGS += -DUSE_HIDDATA
OBJS = ./hiddata.o
OPT_HOME := /opt/local/bin
CFLAGS += `$(OPT_HOME)/libusb-config --cflags`
LIBS   += `$(OPT_HOME)/libusb-config --libs`
endif

LIBS += -framework IOKit -framework CoreFoundation

EXEFLAGS = 
#LIBFLAGS = -bundle -o $(LIBTARGET) -Wl,-search_paths_first $(LIBS)
LIBFLAGS = -dynamiclib -o $(LIBTARGET) -Wl,-search_paths_first $(LIBS)
EXE=

INSTALL = install -D
EXELOCATION ?= /usr/local/bin
LIBLOCATION ?= /usr/local/lib
INCLOCATION ?= /usr/local/include

endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"
LIBTARGET = blink1-lib.dll
#LIBS +=  -mwindows -lsetupapi -Wl,--enable-auto-import -static-libgcc -static-libstdc++ -lkernel32 
#LIBS +=  -mwindows -lsetupapi -Wl,-Bdynamic -lgdi32 -Wl,--enable-auto-import -static-libgcc -static-libstdc++ -lkernel32
LIBS +=             -lsetupapi -Wl,--enable-auto-import -static-libgcc -static-libstdc++ 

ifeq "$(USBLIB_TYPE)" "HIDAPI"
CFLAGS += -DUSE_HIDAPI
CFLAGS += -I./hidapi/hidapi 
OBJS = ./hidapi/windows/hid.o
endif

ifeq "$(USBLIB_TYPE)" "HIDDATA"
CFLAGS += -DUSE_HIDDATA
OBJS = ./hiddata.o
endif

EXEFLAGS =
#LIBFLAGS = -shared -o $(LIBTARGET) -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--out-implib,$(LIBTARGET).a $(LIBS)
LIBFLAGS = -shared -o $(LIBTARGET) -Wl,--add-stdcall-alias -Wl,--export-all-symbols,--output-def,blink1-lib.def,--out-implib,blink1-lib.a
EXE= .exe

# this generates a blink1-lib.lib for use with MSVC
LIB_EXTRA = lib /machine:i386 /def:blink1-lib.def

INSTALL = cp
EXELOCATION ?= $(SystemRoot)/system32
LIBLOCATION ?= $(SystemRoot)/system32
# not sure where this really should point
INCLOCATION ?= $(SystemRoot)/system32

endif

#################  Linux  ####################################################
ifeq "$(OS)" "linux"
LIBTARGET = libblink1.so
# was blink1-lib.so

ifeq "$(USBLIB_TYPE)" "HIDAPI"
CFLAGS += -DUSE_HIDAPI
CFLAGS += -I./hidapi/hidapi 
OBJS = ./hidapi/libusb/hid.o
CFLAGS += `pkg-config libusb-1.0 --cflags` -fPIC
LIBS   += `pkg-config libusb-1.0 --libs` -lrt -lpthread -ldl
endif

ifeq "$(USBLIB_TYPE)" "HIDDATA"
CFLAGS += -DUSE_HIDDATA
OBJS = ./hiddata.o
CFLAGS += `pkg-config libusb --cflags` -fPIC
LIBS   += `pkg-config libusb --libs` 
endif

# static doesn't work on Ubuntu 13+
#EXEFLAGS = -static
LIBFLAGS = -shared -o $(LIBTARGET) $(LIBS)
EXE=

INSTALL = install -D
EXELOCATION ?= /usr/local/bin
LIBLOCATION ?= /usr/local/lib
INCLOCATION ?= /usr/local/include

endif

################  FreeBSD  ###################################################
ifeq "$(OS)" "freebsd"
LIBTARGET = libblink1.so
# was blink1-lib.so

ifeq "$(USBLIB_TYPE)" "HIDAPI"
CFLAGS += -DUSE_HIDAPI
CFLAGS += -I./hidapi/hidapi 
OBJS = ./hidapi/libusb/hid.o
CFLAGS += -I/usr/local/include -fPIC
LIBS   += -lusb -lrt -lpthread
ifndef FBSD10
LIBS   += -L/usr/local/lib -liconv
endif
endif

ifeq "$(USBLIB_TYPE)" "HIDDATA"
CFLAGS += -DUSE_HIDDATA
OBJS = ./hiddata.o
CFLAGS += -I/usr/local/include -fPIC
LIBS   += -L/usr/local/lib -lusb 
endif

# Static binaries don't play well with the iconv implementation of FreeBSD 10
ifndef FBSD10
EXEFLAGS = -static
endif
LIBFLAGS = -shared -o $(LIBTARGET) $(LIBS)
EXE=

INSTALL = install -D
EXELOCATION ?= /usr/local/bin
LIBLOCATION ?= /usr/local/lib
INCLOCATION ?= /usr/local/include

endif

#################  WRT Linux  ################################################
ifeq "$(OS)" "wrt"
LIBTARGET = blink1-lib.so

# HIDAPI build doesn't work, use HIDDATA instead
ifeq "$(USBLIB_TYPE)" "HIDAPI"
CFLAGS += -DUSE_HIDAPI
CFLAGS += -I./hidapi/hidapi 
OBJS = ./hidapi/libusb/hid.o
CFLAGS += `pkg-config libusb-1.0 --cflags` -fPIC 
LIBS   += `pkg-config libusb-1.0 --libs` -lrt -lpthread -ldl 
endif

ifeq "$(USBLIB_TYPE)" "HIDDATA"
CFLAGS += -DUSE_HIDDATA $(COPT_FLAGS)
OBJS = ./hiddata.o
LIBS += $(LDOPT_FLAGS) 
#LIBS += $(STAGING_DIR)/usr/lib/libusb.a 
#can't build this static for some reason
LIBS += -lusb
endif

#EXEFLAGS = -static
LIBFLAGS = -shared -o $(LIBTARGET) $(LIBS)
EXE=

endif

##############  Cross-compile WRT Linux  #####################################
ifeq "$(OS)" "wrtcross"

ifeq "$(USBLIB_TYPE)" "HIDDATA"
CFLAGS += -DUSE_HIDDATA
OBJS = ./hiddata.o

WRT_SDK_HOME := $(HOME)/projects/openwrt/sdk/OpenWrt-SDK-ar71xx-for-Linux-i686-gcc-4.3.3+cs_uClibc-0.9.30.1
CC = $(WRT_SDK_HOME)/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/usr/bin/mips-openwrt-linux-gcc
LD = $(WRT_SDK_HOME)/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/usr/bin/mips-openwrt-linux-ld
CFLAGS += "-I$(WRT_SDK_HOME)/staging_dir/target-mips_r2_uClibc-0.9.30.1/usr/include" -fPIC
LIBS   += "$(WRT_SDK_HOME)/staging_dir/target-mips_r2_uClibc-0.9.30.1/usr/lib/libusb.a"
#LDFLAGS += -static

endif

EXEFLAGS = -static
LIBFLAGS = -shared -o $(LIBTARGET) $(LIBS)
EXE=

endif



#####################  Common  ###############################################


#CFLAGS += -O -Wall -std=gnu99 -I ../hardware/firmware 
CFLAGS += -std=gnu99 
CFLAGS += -g
CFLAGS += -DBLINK1_VERSION=\"$(BLINK1_VERSION)\"

OBJS +=  blink1-lib.o 


PKGOS = $(BLINK1_VERSION)

#all: msg blink1-tool blink1-server-simple
all: msg blink1-tool lib 

# symbolic targets:
help:
	@echo "This Makefile works on multiple archs. Use one of the following:"
	@echo "make            ... autodetect platform and build appropriately"
	@echo "make OS=windows ... build Windows  blink1-lib and blink1-tool" 
	@echo "make OS=linux   ... build Linux    blink1-lib and blink1-tool" 
	@echo "make OS=freebsd ... build FreeBSD    blink1-lib and blink1-tool" 
	@echo "make OS=macosx  ... build Mac OS X blink1-lib and blink1-tool" 
	@echo "make OS=wrt     ... build OpenWrt blink1-lib and blink1-tool"
	@echo "make OS=wrtcross... build for OpenWrt using cross-compiler"
	@echo "make USBLIB_TYPE=HIDDATA OS=linux ... build using low-dep method"
	@echo "make lib        ... build blink1-lib shared library"
	@echo "make blink1-tiny-server ... build tiny REST server"
	@echo "make package    ... zip up blink1-tool and blink1-lib "
	@echo "make clean      ... delete build products, leave binaries & libs"
	@echo "make distclean  ... delele binaries and libs too"
	@echo

msg: 
	@echo "Building for OS=$(OS) BLINK1_VERSION=$(BLINK1_VERSION)"


$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

blink1-tool: $(OBJS) blink1-tool.o
	$(CC) $(CFLAGS) -c blink1-tool.c -o blink1-tool.o
	$(CC) $(CFLAGS) $(EXEFLAGS) -g $(OBJS) $(LIBS) blink1-tool.o -o blink1-tool$(EXE) 

# FIXME: verify we don't need MONGOOSE_LIBS and pthread & dl are avilable everywhere
blink1-tiny-server: $(OBJS) server/blink1-tiny-server.c
	$(CC) $(CFLAGS) -I. -I./server/mongoose -c server/blink1-tiny-server.c -o blink1-tiny-server.o
	$(CC) $(CFLAGS) -I. -I./server/mongoose -c ./server/mongoose/mongoose.c -o ./server/mongoose/mongoose.o
	$(CC) -g $(OBJS) $(EXEFLAGS) ./server/mongoose/mongoose.o $(LIBS) -lpthread -ldl blink1-tiny-server.o -o blink1-tiny-server$(EXE)

lib: $(OBJS)
	$(CC) $(LIBFLAGS) $(CFLAGS) $(OBJS) $(LIBS)
	$(LIB_EXTRA)

package: lib blink1-tool
	@echo "Packaging up blink1-tool and blink1-lib for '$(PKGOS)'"
	zip blink1-tool-$(PKGOS).zip blink1-tool$(EXE)
	zip blink1-lib-$(PKGOS).zip $(LIBTARGET) blink1-lib.h
	@#mkdir -f builds && cp blink1-tool-$(PKGOKS).zip builds

install: all
	$(INSTALL) blink1-tool$(EXE) $(DESTDIR)$(EXELOCATION)/blink1-tool$(EXE)
	$(INSTALL) $(LIBTARGET) $(DESTDIR)$(LIBLOCATION)/$(LIBTARGET)
	$(INSTALL) blink1-lib.h $(DESTDIR)$(INCLOCATION)/blink1-lib.h

.PHONY: install

clean: 
	rm -f $(OBJS)
	rm -f $(LIBTARGET)
	rm -f blink1-tiny-server.o blink1-tool.o hiddata.o
	rm -f server/mongoose/mongoose.o

distclean: clean
	rm -f blink1-tool$(EXE)
	rm -f blink1-tiny-server$(EXE)
	rm -f $(LIBTARGET) $(LIBTARGET).a

# show shared library use
# in general we want minimal to no dependecies for blink1-tool

# shows shared lib usage on Mac OS X
otool:
	otool -L blink1-tool
# show shared lib usage on Linux
ldd:
	ldd blink1-tool
# show shared lib usage on Windows
# FIXME: only works inside command prompt from
# Start->All Programs-> MS Visual Studio 2012 -> VS Tools -> Devel. Cmd Prompt
dumpbin: 
	dumpbin.exe /exports $(LIBTARGET)
	dumpbin.exe /exports blink1-tool.exe


printvars:
	@echo "OS=$(OS), CFLAGS=$(CFLAGS), LDFLAGS=$(LDFLAGS), LIBS=$(LIBS), LIBFLAGS=$(LIBFLAGS)"
