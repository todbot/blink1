# Name: Makefile
# Project: hid-data example
# Author: Christian Starkjohann
# Creation Date: 2008-04-11
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
# This Revision: $Id$

# Please read the definitions below and edit them as appropriate for your
# system:

UNAME ?=$(shell uname)

ifeq "$(UNAME)" "Darwin"
	OS=macosx
endif

ifeq "$(OS)" "Windows_NT"
	OS=windows
endif

ifeq "$(UNAME)" "Linux"
	OS=linux
endif

ifndef OS
#	$(error No OS specified)
endif

$(warning Building for OS='$(OS)')


TARGET = BlinkMUSB

CC=gcc

#################  Mac OS X  ##################################################

ifeq "$(OS)" "macosx"
# Use the following 3 lines on Unix and Mac OS X:
#USBFLAGS=   `libusb-config --cflags`
#USBLIBS=    `libusb-config --libs`
#LIBUSB_CONFIG=/opt/local/bin/libusb-legacy-config
#LIBUSB_CONFIG=/opt/local/bin/libusb-config
# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
#USBFLAGS = `$(LIBUSB_CONFIG) --cflags` 
#USBLIBS = `$(LIBUSB_CONFIG) --libs`

USBFLAGS = `/opt/local/bin/libusb-legacy-config --cflags`
# get just the path to the static lib
USBLIBS = `/opt/local/bin/libusb-legacy-config --libs | cut -d' ' -f1 | cut -c3- `/libusb-legacy.a
# get everything else in --libs
USBLIBS +=  `libusb-legacy-config --libs | cut -d' ' -f 3- `
EXE_SUFFIX=

# to build libusb-legacy for universal on Lion do:
#  sudo port install libusb-legacy configure.compiler=llvm-gcc-4.2  +universal
ARCHS=   -arch i386 -arch x86_64
CFLAGS=	 -O -Wall $(USBFLAGS) $(ARCHS)
CFLAGS+=  -I./mongoose -I../firmware -pthread -g 
LIBS=	 $(USBLIBS) $(ARCHS)

#OS_CFLAGS = -g -O2 -D_BSD_SOURCE -bundle 
#OS_CFLAGS += -isysroot /Developer/SDKs/MacOSX10.6.sdk -mmacosx-version-min=10.6
#OS_LDFLAGS =  -Wl,-search_paths_first -framework JavaVM -framework IOKit -framework CoreFoundation $(USBLIBS) 

JAVAINCLUDEDIR = /System/Library/Frameworks/JavaVM.framework/Headers
JAVANATINC = -I $(JAVAINCLUDEDIR)/./
JAVAINCLUDE = -I $(JAVAINCLUDEDIR)

JAVA_CFLAGS = $(CFLAGS) -bundle 
JAVA_LDFLAGS = $(LDFLAGS)  

JAVA_LIB  = lib$(TARGET).jnilib


# build a static lib:
# libtool -static -o blinkmusb-lib.a  blinkmusb-lib.o hiddata.o /opt/local/lib/libusb-legacy/libusb-legacy.a
# 

endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"
USBFLAGS= 
USBLIBS=    -lhid -lsetupapi 
EXE_SUFFIX= .exe

CFLAGS=	 -O -Wall $(USBFLAGS) -I./mongoose -I../firmware -mthreads

LIBS=	 $(USBLIBS) -lws2_32 -ladvapi32

endif

#################  Common  ##################################################

INCLUDES = -I. $(JAVAINCLUDE) $(JAVANATINC) 

CFLAGS += $(INCLUDES)

OBJ=		blinkmusb-lib.o hiddata.o 
PROGRAM1=	blinkmusb-tool$(EXE_SUFFIX)
PROGRAM2=   blinkmusb-server$(EXE_SUFFIX)


#################  #######  ##################################################

all: $(PROGRAM1) $(PROGRAM2) processing

$(PROGRAM1): $(OBJ) blinkmusb-tool.o
	$(CC) -o $(PROGRAM1) blinkmusb-tool.o $(OBJ)  $(LIBS)

$(PROGRAM2): $(OBJ) blinkmusb-server.o
	$(CC) -o $(PROGRAM2) blinkmusb-server.o mongoose/mongoose.c $(OBJ)  $(LIBS)

lib-mac: $(PROGRAM1)
	libtool -static -o blinkmusb-lib.a  blinkmusb-lib.o hiddata.o /opt/local/lib/libusb-legacy/libusb-legacy.a

javac:
#	javac -target 1.5 thingm/blinkm/BlinkMUSB.java
	javac thingm/blinkm/BlinkMUSB.java

jni:
	which javah
	javah -jni thingm.blinkm.BlinkMUSB

java: javac jni $(OBJ) nativeBlinkMUSB.o
#	$(CC)  -o $(LIBTARGET) $(CFLAGS) $(OBJ) $(LDFLAGS) 
	$(CC) $(JAVA_CFLAGS) -o $(JAVA_LIB) $(LIBS) $(OBJ) nativeBlinkMUSB.o
	mkdir -p libtargets && mv $(JAVA_LIB) libtargets
 

jar: javac jni java
	jar -cfm blinkmusb.jar  packaging/Manifest.txt thingm/blinkm/*.class
	mv blinkmusb.jar libtargets


processing: processinglib
processinglib: jar
	rm -f blinkmusb.zip
	mkdir -p blinkmusb/library
	cp packaging/processing-export.txt blinkmusb/library/export.txt
	cp libtargets/* blinkmusb/library
	zip -r blinkmusb.zip blinkmusb
	@echo
	@echo "now unzip blinkmusb.zip into ~/Documents/Processing/libraries"
#	@echo "or maybe just:\ncp -r blinkmusb ~/Documents/Processing/libraries"
	@echo "or maybe just:\nln -s \`pwd\`/blinkmusb ~/Documents/Processing/libraries/blinkmusb"


.c.o:
	$(CC) $(ARCH_COMPILE) $(CFLAGS) -c $*.c -o $*.o

strip: $(PROGRAM1) $(PROGRAM2)
	strip $(PROGRAM1)
	strip $(PROGRAM2)

clean:
	rm -f $(OBJ) $(PROGRAM1) $(PROGRAM2) *.o *.a *.dll *jnilib 
	rm thingm/blinkm/BlinkMUSB.class

distclean:
	rm -rf blinkmusb
	rm -f libtargets/*


