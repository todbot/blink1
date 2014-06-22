#!/bin/sh
#
# Fade to random colors
# 

while [ 1 ] ; do 
  ((hue = $RANDOM % 255))
  blink1-tool -m 700 --hsb $hue,255,255
  sleep 1
done
