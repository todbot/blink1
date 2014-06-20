#!/bin/sh
#
# Demonstrate how to fill the blink(1) with a particular color pattern
#
#

echo "writing pattern"
blink1-tool -g -m 550 --rgb 0x11,0x11,0x11 --setpattline 0
blink1-tool -g -m 550 --rgb 0x22,0x22,0x22 --setpattline 1
blink1-tool -g -m 550 --rgb 0x33,0x33,0x33 --setpattline 2
blink1-tool -g -m 550 --rgb 0x44,0x44,0x44 --setpattline 3
blink1-tool -g -m 550 --rgb 0x55,0x55,0x55 --setpattline 4
blink1-tool -g -m 550 --rgb 0x66,0x66,0x66 --setpattline 5
blink1-tool -g -m 550 --rgb 0x77,0x77,0x77 --setpattline 6
blink1-tool -g -m 550 --rgb 0x88,0x88,0x88 --setpattline 7
blink1-tool -g -m 550 --rgb 0x99,0x99,0x99 --setpattline 8
blink1-tool -g -m 550 --rgb 0xaa,0xaa,0xaa --setpattline 9
blink1-tool -g -m 550 --rgb 0xbb,0xbb,0xbb --setpattline 10
blink1-tool -g -m 550 --rgb 0xcc,0xcc,0xcc --setpattline 11
blink1-tool -g -m 550 --rgb 0xdd,0xdd,0xdd --setpattline 12
blink1-tool -g -m 550 --rgb 0xee,0xee,0xee --setpattline 13
blink1-tool -g -m 550 --rgb 0xff,0xdd,0xff --setpattline 14
blink1-tool -g -m 550 --rgb 0x00,0x00,0x00 --setpattline 15
blink1-tool --savepattern  # needed for mk2 only

sleep 1

echo "reading back stored pattern"

#n=32  # mk2: 32 (RAM) or 16 (flash) 
n=16  # mk2: 32 (RAM) or 16 (flash) 
#n=12  # mk1: 12
for ((i=0; i<n; i++ )) ; do
  blink1-tool --getpattline $i
done

echo "playing pattern"
blink1-tool --play 1

