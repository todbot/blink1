#!/bin/sh
#
# Another demo of filling the blink(1) with a particular pattern
#

blink1-tool -m 500 --setpattline 255,0,0,0 
blink1-tool -m 500 --setpattline 255,64,0,1 
blink1-tool -m 500 --setpattline 255,128,0,2 
blink1-tool -m 500 --setattline 255,196,0,3 
blink1-tool -m 500 --setpattline 255,255,0,4 
blink1-tool -m 500 --setpattline 196,255,0,5 
blink1-tool -m 500 --setpattline 128,255,0,6 
blink1-tool -m 500 --setpattline 64,255,0,7 
blink1-tool -m 500 --setpattline 0,255,0,8 
blink1-tool -m 500 --setpattline 0,0,0,9 
blink1-tool -m   0 --setpattline 0,0,0,10 
blink1-tool -m   0 --setpattline 0,0,0,11 

blink1-tool --play 1

