#!/usr/bin/env python

"""
blink1_tst -- simple demo of blink1 library

You can also just run blink1_pyusb.py as a blink1-tool replacement

"""

from blink1 import Blink1
import time


blink1 = Blink1()

if( blink1.dev == None ):
    print("no blink1 found")
else: 
    print "blink(1) found"

print "serial number: " + blink1.get_serialnumber()
print "firmware version: " + blink1.get_version()

print "fading to #ffffff"
blink1.fade_to_rgb( 1000,  255,255,255 )

time.sleep(0.5)
        
print "fading to #000000"
blink1.fade_to_rgb( 1000,  0,0,0 )
        
print "done"
