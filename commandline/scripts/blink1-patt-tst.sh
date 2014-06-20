#!/bin/sh
#
# Another demo of filling the blink(1) with a particular pattern
#

blink1-tool -m 500 --rgb 255,0,0   --setpattline 0
blink1-tool -m 500 --rgb 255,64,0  --setpattline 1 
blink1-tool -m 500 --rgb 255,128,0 --setpattline 2 
blink1-tool -m 500 --rgb 255,196,0 --setpattline 3 
blink1-tool -m 500 --rgb 255,255,0 --setpattline 4 
blink1-tool -m 500 --rgb 196,255,0 --setpattline 5 
blink1-tool -m 500 --rgb 128,255,0 --setpattline 6 
blink1-tool -m 500 --rgb  64,255,0 --setpattline 7 
blink1-tool -m 500 --rgb   0,255,0 --setpattline 8 
blink1-tool -m 500 --rgb     0,0,0 --setpattline 9 
blink1-tool -m   0 --rgb     0,0,0 --setpattline 10 
blink1-tool -m   0 --rgb     0,0,0 --setpattline 11 

blink1-tool --play 1

