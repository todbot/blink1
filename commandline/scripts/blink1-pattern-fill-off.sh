#!/bin/sh
#
# Demonstrate how to fill the blink(1) with a particular color pattern
#
#

echo "writing pattern"
blink1-tool  -m 50 --rgb 000000 --setpattline 0
blink1-tool  -m 50 --rgb 000000 --setpattline 1
blink1-tool  -m 50 --rgb 000000 --setpattline 2
blink1-tool  -m 50 --rgb 000000 --setpattline 3
blink1-tool  -m 50 --rgb 000000 --setpattline 4
blink1-tool  -m 50 --rgb 000000 --setpattline 5
blink1-tool  -m 50 --rgb 000000 --setpattline 6
blink1-tool  -m 50 --rgb 000000 --setpattline 7
blink1-tool  -m 50 --rgb 000000 --setpattline 8
blink1-tool  -m 50 --rgb 000000 --setpattline 9
blink1-tool  -m 50 --rgb 000000 --setpattline 10
blink1-tool  -m 50 --rgb 000000 --setpattline 11
blink1-tool  -m 50 --rgb 000000 --setpattline 12
blink1-tool  -m 50 --rgb 000000 --setpattline 13
blink1-tool  -m 50 --rgb 000000 --setpattline 14
blink1-tool  -m 50 --rgb 000000 --setpattline 15
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
