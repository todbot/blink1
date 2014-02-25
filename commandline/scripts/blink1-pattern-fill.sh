#!/bin/sh
#
# Demonstrate how to fill the blink(1) with a particular color pattern
#

blink1-tool -g -m 550  --setpattline 0x11,0x11,0x11,0
blink1-tool -g -m 550  --setpattline 0x22,0x22,0x22,1
blink1-tool -g -m 550  --setpattline 0x33,0x33,0x33,2
blink1-tool -g -m 550  --setpattline 0x44,0x44,0x44,3
blink1-tool -g -m 550  --setpattline 0x55,0x55,0x55,4
blink1-tool -g -m 550  --setpattline 0x66,0x66,0x66,5
blink1-tool -g -m 550  --setpattline 0x77,0x77,0x77,6
blink1-tool -g -m 550  --setpattline 0x88,0x88,0x88,7
blink1-tool -g -m 550  --setpattline 0x99,0x99,0x99,8
blink1-tool -g -m 550  --setpattline 0xaa,0xaa,0xaa,9
blink1-tool -g -m 550  --setpattline 0xbb,0xbb,0xbb,10
blink1-tool -g -m 550  --setpattline 0xcc,0xcc,0xcc,11
blink1-tool -g -m 550  --setpattline 0xdd,0xdd,0xdd,12
blink1-tool -g -m 550  --setpattline 0xee,0xee,0xee,13
blink1-tool -g -m 550  --setpattline 0xff,0xdd,0xff,14
blink1-tool -g -m 550  --setpattline 0x00,0x00,0x00,15
blink1-tool -g --savepattern  # needed for mk2 only


