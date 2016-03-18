#!/bin/sh
#
# Do something with multiple blink1s
#

num_blink1s=4

for h in {0..15} ; do 
    ((hue=h*16))
    echo "hue=$hue"
    for ((i=0; i<num_blink1s; i++ )) ; do
        blink1-tool -d $i --hsb $hue,255,255
    done
    hue_old=$hue
    sleep 0.3
  done
done

