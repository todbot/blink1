"""

blink1.py -- blink(1) Python library

Uses PyUSB to do direct USB HID commands

See: https://github.com/walac/pyusb
 or "apt-get install pyusb" on linux
 or "brew install libusb" on osx 
 or "port install py26-pyusb-devel" on osx
 or libusb-win32 (inf method) on windows

Based on blink1hid-demo.py by Aaron Blondeau

2013, Tod E. Kurt, http://thingm.com/

"""

import time
import string

import usb 

class Blink1:

    def __init__(self):
        return self.find()
    
    def find(self):
        self.dev = usb.core.find(idVendor=0x27b8, idProduct=0x01ed)
        if( self.dev == None ): return None

    def notfound(self):
        return None  # fixme what to do here

    def fadeToRGB(self, red,green,blue, fadeMillis, ledn):
        if( self.dev == None ): return self.notfound()
        bmRequestTypeOut = usb.util.build_request_type(usb.util.CTRL_OUT, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        #action = 0x6E # ='n' (set rgb now)
        action = 0x63 # ='c' (fade to rgb)
        fadeMillis = fadeMillis/10
        th = (fadeMillis & 0xff00) >> 8
        tl = fadeMillis & 0x00ff
        self.dev.ctrl_transfer(bmRequestTypeOut, 0x09, (3 << 8) | 0x01, 0, 
                              [0x01, action, red, green, blue, th,tl, ledn])

#get version number
    def getVersion(self):
        if( self.dev == None ): return ''
        action = 0x76 # ='v' (version)
        bmRequestTypeOut = usb.util.build_request_type(usb.util.CTRL_OUT, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        self.dev.ctrl_transfer(bmRequestTypeOut, 0x09, (3 << 8) | 0x01, 0, 
                               [0x00, action, 0x00,0x00,0x00,0x00,0x00,0x00,0x00])
        time.sleep(.05)
        bmRequestTypeIn = usb.util.build_request_type(usb.util.CTRL_IN, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        version_raw = self.dev.ctrl_transfer(bmRequestTypeIn, 0x01, (3 << 8) | 0x01, 0, 8)
        version = ''.join(chr(i) for i in version_raw) 
        version = filter(lambda x: x in string.printable, version)
        return version


def main():
    blink1 = Blink1()
    if( blink1.dev == None ):
        print("no blink1 found")

    print "blink1 version: "+ blink1.getVersion()
        
    demoColors = [ [255,  0,  0],  # red
                   [  0,255,  0],  # grn
                   [  0,  0,255],  # blu
                   [255,255,  0],  # yellow
                   [  0,255,255],  # cyan
                   [255,  0,255],  # magenta
                   [  0,  0,  0],  # off
                   ]
        
    fadeMillis = 100
    
    for rgb in demoColors:
        r = rgb[0]
        g = rgb[1]
        b = rgb[2]
        print "fading to %3i,%3i,%3i" % (r,g,b)
        blink1.fadeToRGB( r,g,b, fadeMillis, 0 )
        time.sleep(0.5)
        
        blink1.fadeToRGB(   0,0,0, 1000, 0 )


if __name__ == '__main__':
    main()


