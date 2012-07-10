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


LIBHID_STYLE=obdev
#LIBHID_STYLE=native

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


TARGET = Blink1

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
CFLAGS=	 $(USBFLAGS) $(ARCHS)
CFLAGS+=  -I./mongoose -I../firmware -pthread -g 
LIBS=	 $(USBLIBS) $(ARCHS)

ifeq "$(LIBHID_STYLE)" "obdev"
CFLAGS+=  -I./libhid_obdevstyle
else
CFLAGS+=  -I./nativehid
endif

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
# libtool -static -o blink1-lib.a  blink1-lib.o hiddata.o /opt/local/lib/libusb-legacy/libusb-legacy.a
# 

endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"
USBFLAGS= 
USBLIBS=    -lhid -lsetupapi 
EXE_SUFFIX= .exe

CFLAGS=	 $(USBFLAGS) -I./mongoose -I../firmware -mthreads

LIBS=	 $(USBLIBS) -lws2_32 -ladvapi32

endif

################## Linux ####################################################
ifeq "$(OS)" "linux"
USBFLAGS =   `libusb-config --cflags`
USBLIBS  =   `libusb-config --libs`
EXE_SUFFIX=
CFLAGS+=  $(USBFLAGS) -I./mongoose -I../firmware -pthread -g 
LIBS = $(USBLIBS)

endif

#################  Common  ##################################################

INCLUDES = -I. $(JAVAINCLUDE) $(JAVANATINC) 

CFLAGS += $(INCLUDES) -O -Wall -std=gnu99 

OBJ=		blink1-lib.o hiddata.o 

PROGRAM1=	blink1-tool$(EXE_SUFFIX)
PROGRAM2=   blink1-server$(EXE_SUFFIX)


#################  #######  ##################################################

all: 
	@echo "Available targets for this Makefile:"
	@echo " make $(PROGRAM1) -- build $(PROGRAM1) (and library)"
	@echo " make $(PROGRAM2) -- build $(PROGRAM2) (and library)" 
	@echo " make jar         -- build Java library"
	@echo " make processing  -- build library, Java lib, and Processsing lib"

$(PROGRAM1): $(OBJ) blink1-tool.o
	$(CC) -o $(PROGRAM1) blink1-tool.o $(CFLAGS) $(OBJ)  $(LIBS)

$(PROGRAM2): $(OBJ) blink1-server.o
	$(CC) -o $(PROGRAM2) blink1-server.o mongoose/mongoose.c $(OBJ)  $(LIBS)

lib-mac: $(PROGRAM1)
	libtool -static -o blink1-lib.a  blink1-lib.o hiddata.o /opt/local/lib/libusb-legacy/libusb-legacy.a

javac:
#	javac -target 1.5 thingm/blink1/Blink1.java
	javac thingm/blink1/Blink1.java

jni:
	which javah
	javah -jni thingm.blink1.Blink1

java: javac jni $(OBJ) nativeBlink1.o
#	$(CC)  -o $(LIBTARGET) $(CFLAGS) $(OBJ) $(LDFLAGS) 
	$(CC) $(JAVA_CFLAGS) -o $(JAVA_LIB) $(LIBS) $(OBJ) nativeBlink1.o
	mkdir -p libtargets && mv $(JAVA_LIB) libtargets
 

jar: javac jni java
	jar -cfm blink1.jar  packaging/Manifest.txt thingm/blink1/*.class
	mv blink1.jar libtargets


processing: processinglib
processinglib: jar
	rm -f blink1.zip
	mkdir -p blink1/library
	cp packaging/processing-export.txt blink1/library/export.txt
	cp libtargets/* blink1/library
	zip -r blink1.zip blink1
	@echo
	@echo "now unzip blink1b.zip into ~/Documents/Processing/libraries"
#	@echo "or maybe just:\ncp -r blink1 ~/Documents/Processing/libraries"
	@echo "or maybe just:\nln -s \`pwd\`/blink1 ~/Documents/Processing/libraries/blink1"


.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o
#	$(CC) $(ARCH_COMPILE) $(CFLAGS) -c $*.c -o $*.o

strip: $(PROGRAM1) $(PROGRAM2)
	strip $(PROGRAM1)
	strip $(PROGRAM2)

clean:
	rm -f $(OBJ) $(PROGRAM1) $(PROGRAM2) *.o *.a *.dll *jnilib 
	rm -f thingm/blink1/Blink1.class

distclean: clean
	rm -rf blink1
#	rm -f libtargets/*
	rm -rf libtargets


