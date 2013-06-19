# Makefile for "blink1-lib" and "blink1-tool"
# should work on Mac OS X, Windows, Linux, and other Linux-like systems
#  
# Build arguments:
# - "OS=macosx"  -- build Mac version on Mac OS X
# - "OS=windows" -- build Windows version on Windows
# - "OS=linux"   -- build Linux version on Linux
# 
# Architecture is usually detected automatically, so normally just type "make"
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
# FreeBSD
#   - libusb is part of the OS so no pkg-config needed.
#   - No -ldl on FreeBSD necessary.
#   - iconv is a package that needs to be installed; it lives in /usr/local/lib/
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

ifeq "$(PKGOS)" ""
   PKGOS = $(OS)
endif


CC=gcc
#CC=clang

#################  Mac OS X  ##################################################
ifeq "$(OS)" "macosx"
LIBTARGET = libBlink1.dylib
CFLAGS += -arch i386 -arch x86_64
CFLAGS += -pthread
CFLAGS += -mmacosx-version-min=10.6
LIBS += -framework IOKit -framework CoreFoundation
OBJS = ./hidapi/mac/hid.o

EXEFLAGS = 
#LIBFLAGS = -bundle -o $(LIBTARGET) -Wl,-search_paths_first $(LIBS)
LIBFLAGS = -dynamiclib -o $(LIBTARGET) -Wl,-search_paths_first $(LIBS)

EXE=
endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"
#LIBTARGET = libBlink1.dll
LIBTARGET = blink1-lib.dll
#LIBS +=  -mwindows -lsetupapi -Wl,--enable-auto-import -static-libgcc -static-libstdc++ -lkernel32 
#LIBS +=  -mwindows -lsetupapi -Wl,-Bdynamic -lgdi32 -Wl,--enable-auto-import -static-libgcc -static-libstdc++ -lkernel32
LIBS +=             -lsetupapi -Wl,--enable-auto-import -static-libgcc -static-libstdc++ 
OBJS = ./hidapi/windows/hid.o

EXEFLAGS =
#LIBFLAGS = -shared -o $(LIBTARGET) -Wl,--add-stdcall-alias -Wl,--export-all-symbols -Wl,--out-implib,$(LIBTARGET).a $(LIBS)
LIBFLAGS = -shared -o $(LIBTARGET) -Wl,--add-stdcall-alias -Wl,--export-all-symbols

EXE= .exe
endif

#################  Linux  ####################################################
ifeq "$(OS)" "linux"
LIBTARGET = blink1-lib.so
#LIBTARGET = libBlink1.so
CFLAGS += `pkg-config libusb-1.0 --cflags` -fPIC
LIBS   += `pkg-config libusb-1.0 --libs` -lrt -lpthread -ldl

OBJS = ./hidapi/libusb/hid.o

EXEFLAGS = -static
LIBFLAGS = -shared -o $(LIBTARGET) $(LIBS)

EXE=
endif

################  FreeBSD  ###################################################
ifeq "$(OS)" "freebsd"
LIBTARGET = blink1-lib.so
#LIBTARGET = libBlink1.so
LIBS   += -L/usr/local/lib -lusb -lrt -lpthread -liconv -static
OBJS = ./hidapi/libusb/hid.o
EXEFLAGS=
LIBFLAGS = -shared -o $(LIBTARGET) $(LIBS)
EXE=
endif


#####################  Common  ###############################################

CFLAGS += -std=gnu99 -I ../hardware/firmware 
#CFLAGS += -O -Wall -std=gnu99 -I ../hardware/firmware 
CFLAGS += -I./hidapi/hidapi -I./mongoose -g
# only uncomment for debugging HIDAPI stuff
#CFLAGS += -DDEBUG_PRINTF

OBJS +=  blink1-lib.o 

#all: msg blink1-tool blink1-server-simple
all: msg blink1-tool lib

msg: 
	@echo "building for OS=$(OS)"

# symbolic targets:
help:
	@echo "This Makefile works on multiple archs. Use one of the following:"
	@echo "make OS=windows ... build Windows  blink1-lib and blink1-tool" 
	@echo "make OS=linux   ... build Linux    blink1-lib and blink1-tool" 
	@echo "make OS=freebsd ... build FreeBSD    blink1-lib and blink1-tool" 
	@echo "make OS=macosx  ... build Mac OS X blink1-lib and blink1-tool" 
	@echo "make lib        ... build blink1-lib shared library"
	@echo "make package PKGOS=mac  ... zip up build, give it a name 'mac' "
	@echo "make clean ..... to delete objects and hex file"
	@echo

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

blink1-tool: $(OBJS) blink1-tool.o
	$(CC) $(CFLAGS) -c blink1-tool.c -o blink1-tool.o
	$(CC) $(CFLAGS) $(EXEFLAGS) -g $(OBJS) $(LIBS) blink1-tool.o -o blink1-tool$(EXE) 

blink1-server-simple: $(OBJS) blink1-server-simple.c
	$(CC) $(CFLAGS) -c blink1-server-simple.c -o blink1-server-simple.o
	$(CC) $(CFLAGS) -c ./mongoose/mongoose.c -o ./mongoose/mongoose.o
	$(CC) -g $(OBJS) $(EXEFLAGS) ./mongoose/mongoose.o $(LIBS) blink1-server-simple.o -o blink1-server-simple$(EXE)

lib: $(OBJS)
	$(CC) $(LIBFLAGS) $(CFLAGS) $(OBJS) $(LIBS)


package: blink1-tool
	@echo "Zipping up blink1-tool for '$(PKGOS)'"
	zip ../builds/blink1-tool-$(PKGOS).zip blink1-tool$(EXE)

clean: 
	rm -f $(OBJS) 
	rm -f blink1-server-simple.o blink1-tool.o
	rm -f mongoose/mongoose.o

distclean: clean
	rm -f blink1-tool$(EXE)
	rm -f blink1-server-simple$(EXE)
	rm -f $(LIBTARGET) $(LIBTARGET).a

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

foo:
	@echo "OS=$(OS), USBFLAGS=$(USBFLAGS)"

