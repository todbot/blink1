#!/bin/bash
#
# play an infinite shifing rainbow
# 

while [ 1 ] ;  do
  for h in {0..15}
  do
    ((hue=h*16))
    echo "hue=$hue"
    blink1-tool -l 2 --hsb $hue,255,255
    blink1-tool -l 1 --hsb $hue_old,255,255
    hue_old=$hue
    sleep 0.3
  done
done
