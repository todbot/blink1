#!/usr/bin/python
"""
blink1hid-demo.py -- Low-level HID access with Python
 
Thanks to Aaron Blondeau for this

As he says:
 "I am working on controlling a Blink1 from an Android.  
 In order to understand how the set feature calls work without wearing out 
 the usb port on my Android I have created a python script that uses the pyusb library.
 The code is as follows if you wish to share and save some time for other folks."

"""

import usb # https://github.com/walac/pyusb
          # + "brew install libusb" on osx 
          # + libusb-win32 (inf method) on windows

#Find the Blink1
dev = usb.core.find(idVendor=0x27b8, idProduct=0x01ed)
assert dev is not None

#The Blink1 takes 8 bytes of input
# 1=report_id (0)
# 2=action (c = fade to rgb, n = set rgb now)
# 3=red
# 4=green
# 5=blue
# 6=th : time/cs high (T >>8)  where time 'T' is a number of 10msec ticks
# 7=tl : time/cs low (T & 0xff)
# 8=step (0)

# once a buffer is set with these bytes, we need to do what blink1_write / hid_send_feature_report does
# https://github.com/todbot/blink1/blob/master/commandline/blink1-lib.c
# https://github.com/signal11/hidapi/blob/master/libusb/hid.c

#set color to red
bmRequestTypeOut = usb.util.build_request_type(usb.util.CTRL_OUT, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
action = 0x6E # ='n' (set rgb now)
red = 0xFF
green = 0x00
blue = 0x00
dev.ctrl_transfer(bmRequestTypeOut, 0x09, (3 << 8) | 0x01, 0, 
                  [0x00, action, red, green, blue, 0x00, 0x00, 0x00])

#then fade to blue
action = 0x63 # ='c' (fade to rgb)
red = 0x00
green = 0x00
blue = 0xFF
T = 2000/10 #5 seconds worth of 10msec tics
th = (T & 0xff00) >> 8
tl = T & 0x00ff
dev.ctrl_transfer(bmRequestTypeOut, 0x09, (3 << 8) | 0x01, 0, 
                  [0x00, action, red, green, blue, th, tl, 0x00])

#get version number
import time
import string
action = 0x76 # ='v' (version)
dev.ctrl_transfer(bmRequestTypeOut, 0x09, (3 << 8) | 0x01, 0, 
                  [0x00, action, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
time.sleep(.05)
bmRequestTypeIn = usb.util.build_request_type(usb.util.CTRL_IN, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
version_raw = dev.ctrl_transfer(bmRequestTypeIn, 0x01, (3 << 8) | 0x01, 0, 8)
version = ''.join(chr(i) for i in version_raw) # items in the array should correspond to ascii codes for something like "v 100"
version = filter(lambda x: x in string.printable, version)
print version #the c code must tack on an extra 0?

