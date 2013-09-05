#!/bin/sh
#
# Demonstrate last-/current-color readback
# 

blink1-tool --off
sleep 1
blink1-tool --rgb 0xff,0xcc,0xba -m 3000

for i in {1..100} ; do
  blink1-tool --rgbread  # returns last rgb cmd on mk2, curr color on mk2
  sleep 0.1
done
