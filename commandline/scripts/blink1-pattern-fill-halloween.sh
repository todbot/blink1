#!/bin/sh
#
# Demonstrate how to fill the blink(1) with a particular color pattern
#
#

echo "writing pattern"
blink1-tool  -m 550 --rgb 000011 --setpattline 0
blink1-tool  -m 550 --rgb 660066 --setpattline 1
blink1-tool  -m 250 --rgb 8C264D --setpattline 2
blink1-tool  -m 550 --rgb B34D33 --setpattline 3
blink1-tool  -m 550 --rgb D9731A --setpattline 4
blink1-tool  -m 550 --rgb FF9900 --setpattline 5
blink1-tool  -m 550 --rgb 777777 --setpattline 6
blink1-tool  -m 1550 --rgb FF9900 --setpattline 7
blink1-tool  -m  50 --rgb 000000 --setpattline 8
blink1-tool  -m 550 --rgb FF9900 --setpattline 9
blink1-tool  -m 3550 --rgb FF9900 --setpattline 10
blink1-tool  -m 550 --rgb 8C264D --setpattline 11
blink1-tool  -m 250 --rgb 660066 --setpattline 12
blink1-tool  -m 950 --rgb FF9900 --setpattline 13
blink1-tool  -m 550 --rgb FFFFFF --setpattline 14
blink1-tool  -m  50 --rgb 000000 --setpattline 15
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

