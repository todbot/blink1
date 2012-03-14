# BlinkM USB Makefile
# 
#
# 2012, Tod E. Kurt, http://thingm.com/  http://todbot.com/blog/
#
# Originally from EasyLogger by Christian Starkjohann
# License: GPLv2.
#

TARGET=blinkm_usb

#DEVICE=attiny85
DEVICE=attiny45
# PLL
F_CPU=16500000
# non-PLL
#F_CPU=8000000
#F_CPU=12800000

# set fuse based on clock freq (and mayb other things)
ifeq ($(F_CPU),16500000)
FUSE_H=0xdd
FUSE_L=0xe1
endif
ifeq ($(F_CPU),8000000)
FUSE_H=0xdd
FUSE_L=0xe2
endif

AVRDUDE = avrdude -c avrispmkii -P usb -p $(DEVICE)
# The two lines above are for "avrdude" and the STK500 programmer connected
# to an USB to serial converter to a Mac running Mac OS X.
# Choose your favorite programmer and interface.

COMPILE = avr-gcc -std=gnu99 -Wall -Os -Iusbdrv -I.      -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -DDEBUG_LEVEL=0
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o $(TARGET).o

# symbolic targets:
all:	$(TARGET).hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:$(TARGET).hex:i
#	$(AVRDUDE) -D -U flash:w:$(TARGET).hex:i

program: flash

# ATtiny85
# Fuse high byte:
# 0xdd = 1 1 0 1   1 1 0 1
#        ^ ^ ^ ^   ^ \-+-/ 
#        | | | |   |   +------ BODLEVEL 2..0 (brownout trigger level -> 2.7V)
#        | | | |   +---------- EESAVE (preserve EEPROM on Chip Erase -> not preserved)
#        | | | +-------------- WDTON (watchdog timer always on -> disable)
#        | | +---------------- SPIEN (enable serial programming -> enabled)
#        | +------------------ DWEN (debug wire enable)
#        +-------------------- RSTDISBL (disable external reset -> enabled)
#
# Fuse low byte:
# 0xe1 = 1 1 1 0   0 0 0 1
#        ^ ^ \+/   \--+--/
#        | |  |       +------- CKSEL 3..0 (clock selection -> HF PLL)
#        | |  +--------------- SUT 1..0 (BOD enabled, fast rising power)
#        | +------------------ CKOUT (clock output on CKOUT pin -> disabled)
#        +-------------------- CKDIV8 (divide clock by 8 -> don't divide)
fuse:
# PLL
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m
# non-PLL
#	$(AVRDUDE) -U hfuse:w:0xdd:m -U lfuse:w:0xe2:m


readfuse: getfuse
getfuse:
	rm -f hfuse.txt lfuse.txt efuse.txt lockfuse.txt
	$(AVRDUDE) -U hfuse:r:/dev/stdout:h -U lfuse:r:/dev/stdout:h 

readcal:
	$(AVRDUDE) -U calibration:r:/dev/stdout:h
#	$(AVRDUDE) -U calibration:r:/dev/stdout:i | head -1


clean:
	rm -f $(TARGET).hex $(TARGET).lst $(TARGET).obj $(TARGET).cof $(TARGET).list $(TARGET).map $(TARGET).eep.hex $(TARGET).bin *.o usbdrv/*.o $(TARGET).s usbdrv/oddebug.s usbdrv/usbdrv.s

# file targets:
$(TARGET).bin:	$(OBJECTS)
	$(COMPILE) -o $(TARGET).bin $(OBJECTS)

$(TARGET).hex:	$(TARGET).bin
	rm -f $(TARGET).hex $(TARGET).eep.hex
	avr-objcopy -j .text -j .data -O ihex $(TARGET).bin $(TARGET).hex
	avr-size --mcu=$(MCU) --format=avr $(TARGET).bin
	#./checksize $(TARGET).bin 4096 256
# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

disasm:	$(TARGET).bin
	avr-objdump -d $(TARGET).bin

cpp:
	$(COMPILE) -E $(TARGET).c
