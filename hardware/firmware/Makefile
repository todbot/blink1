# blink(1) firmware Makefile
#
# To build the firmware, you need the avr-gcc compiler.
# On Linux, that's: 
#   - apt-get install gcc-avr avr-libc   (to build firmware files)
#
#
# 2012, Tod E. Kurt, http://thingm.com/  http://todbot.com/blog/
#
# Originally from EasyLogger by Christian Starkjohann
# License: GPLv2.
#

TARGET=blink1

MCU=attiny85
#MCU=attiny45
# PLL
F_CPU=16500000
# non-PLL
#F_CPU=8000000
#F_CPU=12800000

# set fuse based on clock freq (and maybe other things)
ifeq ($(F_CPU),16500000)
FUSE_H=0xdd
FUSE_L=0xe1
endif
ifeq ($(F_CPU),8000000)
FUSE_H=0xdd
FUSE_L=0xe2
endif

PKGDATE = `date +%Y%m%d`

AVRDUDE = avrdude -c avrispmkii -P usb -p $(MCU)
# The two lines above are for "avrdude" and the STK500 programmer connected
# to an USB to serial converter to a Mac running Mac OS X.
# Choose your favorite programmer and interface.

COMPILE = avr-gcc -std=gnu99 -Wall -Os -Iusbdrv -I. -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DDEBUG_LEVEL=0
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o $(TARGET).o

# symbolic targets:
all:	$(TARGET).hex $(TARGET).eep

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
	$(AVRDUDE) -U flash:w:$(TARGET).hex:i -U eeprom:w:$(TARGET).eep
#	$(AVRDUDE) -D -U flash:w:$(TARGET).hex:i

program: flash 

# FIXME: i can't makefile
$(TARGET).eep: $(TARGET).bin
	@echo
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O ihex $< $@


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
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m


readfuse: getfuse
getfuse:
	$(AVRDUDE) -U hfuse:r:-:h -U lfuse:r:-:h 

readcal:
	$(AVRDUDE) -U calibration:r:-:h

readeep: 
	$(AVRDUDE) -U  eeprom:r:-:h


clean:
	rm -f $(TARGET).hex $(TARGET).lst $(TARGET).obj $(TARGET).cof $(TARGET).list $(TARGET).map $(TARGET).eep.hex $(TARGET).bin *.o usbdrv/*.o $(TARGET).s usbdrv/oddebug.s usbdrv/usbdrv.s 

# file targets:
$(TARGET).bin:	$(OBJECTS)
	$(COMPILE) -o $(TARGET).bin $(OBJECTS)

$(TARGET).hex:	$(TARGET).bin
	rm -f $(TARGET).hex $(TARGET).eep.hex
	avr-objcopy -j .text -j .data -O ihex $(TARGET).bin $(TARGET).hex
	avr-size --mcu=$(MCU) --format=avr $(TARGET).bin

disasm:	$(TARGET).bin
	avr-objdump -d $(TARGET).bin

cpp:
	$(COMPILE) -E $(TARGET).c


packaging:
	echo "making $(TARGET)_$(PKGDATE)"
	mkdir -p $(TARGET)_$(PKGDATE)
	cp -f $(TARGET).hex $(TARGET)_$(PKGDATE)/$(TARGET)_$(PKGDATE).hex
	cp -f $(TARGET).eep $(TARGET)_$(PKGDATE)/$(TARGET)_$(PKGDATE).eep
	cp -f $(TARGET)_$(MCU)_fuse.txt $(TARGET)_$(PKGDATE)
	cp -f $(TARGET)_eeprom_layout.txt $(TARGET)_$(PKGDATE)
	zip -r $(TARGET)_$(PKGDATE).zip $(TARGET)_$(PKGDATE)
