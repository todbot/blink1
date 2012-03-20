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


CC=gcc

#################  Mac OS X  ##################################################

ifeq "$(OS)" "macosx"
# Use the following 3 lines on Unix and Mac OS X:
#USBFLAGS=   `libusb-config --cflags`
#USBLIBS=    `libusb-config --libs`
LIBUSB_CONFIG=/opt/local/bin/libusb-legacy-config
#LIBUSB_CONFIG=/opt/local/bin/libusb-config
# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
USBFLAGS = `$(LIBUSB_CONFIG) --cflags` 
USBLIBS = `$(LIBUSB_CONFIG) --libs`
EXE_SUFFIX=

CFLAGS=			-O -Wall $(USBFLAGS) -I./mongoose -I../firmware -pthread -g
LIBS=			$(USBLIBS) 

endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"
USBFLAGS= 
USBLIBS=    -lhid -lsetupapi 
EXE_SUFFIX= .exe

CFLAGS=			-O -Wall $(USBFLAGS) -I./mongoose -I../firmware -mthreads
LIBS=			$(USBLIBS) -lws2_32 -ladvapi32

endif


OBJ=		blinkmusb-lib.o hiddata.o 
PROGRAM1=	blinkmusb-tool$(EXE_SUFFIX)
PROGRAM2=   blinkmusb-server$(EXE_SUFFIX)

#################  #######  ##################################################

all: $(PROGRAM1) $(PROGRAM2)

$(PROGRAM1): $(OBJ) blinkmusb-tool.o
	$(CC) -o $(PROGRAM1) blinkmusb-tool.o $(OBJ)  $(LIBS)

$(PROGRAM2): $(OBJ) blinkmusb-server.o
	$(CC) -o $(PROGRAM2) blinkmusb-server.o mongoose/mongoose.c $(OBJ)  $(LIBS)

strip: $(PROGRAM1) $(PROGRAM2)
	strip $(PROGRAM1)
	strip $(PROGRAM2)

clean:
	rm -f $(OBJ) $(PROGRAM1) $(PROGRAM2) blinkmusb-server.o blinkmusb-tool.o

.c.o:
	$(CC) $(ARCH_COMPILE) $(CFLAGS) -c $*.c -o $*.o

