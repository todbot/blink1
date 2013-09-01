#!/usr/bin/env python

"""

blink1_pyusb.py -- blink(1) Python library using PyUSB

Uses "PyUSB 1.0" to do direct USB HID commands

See: https://github.com/walac/pyusb

Linux (Ubuntu/Debian):
 % sudo apt-get install pip
 % sudo pip install pyusb
 Note: will give "not claimed" error or similar.  Try blink1.py instead

Mac OS X:
 do "brew install libusb" on osx 
 or "port install py26-pyusb-devel" on osx

Windows:
 libusb-win32 (inf method) on windows?

Based on blink1hid-demo.py by Aaron Blondeau

2013, Tod E. Kurt, http://thingm.com/

"""

import usb 
import time
import string
import re


report_id = 0x01
debug_rw = False

class Blink1:

    def __init__(self):
        return self.find()
    
    def find(self):
        self.dev = usb.core.find(idVendor=0x27b8, idProduct=0x01ed)
        if( self.dev == None ): 
            return None

        #print "kernel_driver_active: %i" % (self.dev.is_kernel_driver_active(0))
        if( self.dev.is_kernel_driver_active(0) ):
            try:
                self.dev.detach_kernel_driver(0)
            except usb.core.USBError as e:
                sys.exit("Could not detatch kernel driver: %s" % str(e))
        #self.dev.set_configuration()
            
    def notfound(self):
        return None  # fixme what to do here

    """
    Write command to blink(1)
    Send USB Feature Report 0x01 to blink(1) with 8-byte payload
    Note: arg 'buf' must be 8 bytes or bad things happen
    """
    def write(self,buf):
        if debug_rw : print "blink1write: " + ",".join( '0x%02x' % v for v in buf )
        if( self.dev == None ): return self.notfound()
        bmRequestTypeOut = usb.util.build_request_type(usb.util.CTRL_OUT, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        self.dev.ctrl_transfer( bmRequestTypeOut, 
                                0x09,    # == HID set_report
                                (3 << 8) | report_id,  # (3==HID feat.report)
                                0, 
                                buf) 
        
    """
    Read command result from blink(1)
    Receive USB Feature Report 0x01 from blink(1) with 8-byte payload
    Note: buf must be 8 bytes or bad things happen
    """
    def read(self):
        bmRequestTypeIn = usb.util.build_request_type(usb.util.CTRL_IN, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        buf = self.dev.ctrl_transfer( bmRequestTypeIn, 
                                      0x01,  # == HID get_report
                                      (3 << 8) | report_id, 
                                      0, 
                                      8 )    # == number of bytes to read
        if debug_rw : print "blink1read:  " + ",".join( '0x%02x' % v for v in buf )
        return buf

    """
    Command blink(1) to fade to RGB color

    """
    def fade_to_rgbn(self, fadeMillis, red,green,blue, ledn):
        action = ord('c')
        fadeMillis = fadeMillis/10
        th = (fadeMillis & 0xff00) >> 8
        tl = fadeMillis & 0x00ff
        buf = [report_id, action, red,green,blue, th,tl, ledn]
        return self.write(buf)

    """
    Command blink(1) to fade to RGB color

    """
    def fade_to_rgb(self, fadeMillis, red,green,blue):
        return self.fade_to_rgbn(fadeMillis, red,green,blue,0)

    """
    """
    def playloop(self, play,startpos,endpos,count):
        buf = [0x01, ord('p'), play, startpos, endpos, count, 0,0 ]
        return self.write(buf)

    """
    """
    def play(self, play,startpos):
        return self.playloop( play, startpos, 0,0)

    """
    Get blink(1) firmware version

    """
    def get_version(self):
        if( self.dev == None ): return ''
        action = ord('v') # 0x76 # ='v' (version)
        buf = [0x01, action, 0,0, 0,0,0,0]
        self.write(buf)
        time.sleep(.05)
        #bmRequestTypeIn = usb.util.build_request_type(usb.util.CTRL_IN, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        #version_raw = self.dev.ctrl_transfer(bmRequestTypeIn, 0x01, (3 << 8) | 0x01, 0, 8)
        version_raw = self.read()
        version = (version_raw[3]-ord('0'))*100 + (version_raw[4]-ord('0'))
        return str(version)


##################################################################################

#
def parse_color_string(rgbstr):
    rgbstr = rgbstr.lower()
    rgb = None
    # match hex color code "#FFcc00"
    m = re.search(r"#([0-9a-f]{6})", rgbstr)
    if m:
        rgb = tuple(ord(c) for c in m.group(1).decode('hex'))
    else:
        # match color triplets like "255,0,0" and "0xff,0xcc,0x33"
        m = re.search(r"(0x[\da-f]+|\d+),(0x[\da-f]+|\d+),(0x[\da-f]+|\d+)", rgbstr)
        if m:
            rgb = tuple(int(c,0) for c in m.groups())
            
    return rgb

#
def demo(blink1):

    print "blink1 version: "+ blink1.get_version()
        
    democolors = [ [255,  0,  0],  # red
                   [  0,255,  0],  # grn
                   [  0,  0,255],  # blu
                   [255,255,  0],  # yellow
                   [  0,255,255],  # cyan
                   [255,  0,255],  # magenta
                   [  0,  0,  0],  # off
                   ]

    demo_millis = 200

    for rgb in democolors:
        (r,g,b) = map(int,rgb)

        print "fading to %3i,%3i,%3i" % (r,g,b)
        blink1.fade_to_rgbn( demo_millis/2, r,g,b, 0 )
        time.sleep( demo_millis/1000.0 ) 
        blink1.fade_to_rgbn( demo_millis/2, 0,0,0, 0 )
        time.sleep( demo_millis/1000.0 ) 


if __name__ == '__main__':

    from optparse import OptionParser
    
    parser = OptionParser()
    parser.add_option('--demo',    
                      action='store_const', dest='cmd',const='demo',
                      help='run simple demo')

    parser.add_option('--version', 
                      action='store_const', dest='cmd',const='version',
                      help='return firmware version')

    parser.add_option('--blink', 
                      dest='blink',default=0, type='int',
                      help='blink specified number of times')

    parser.add_option('--play', 
                      dest='play',default=0, type='string',
                      help='play built-in light sequence')

    parser.add_option('--rgb', dest='rgb', default='',
                      help="the RGB color to use")

    parser.add_option('-l', '--led', dest='ledn', default=0, type='int',
                      help="which LED to use (default=both)")

    parser.add_option('-m', '--millis', dest='fade_millis', default=300, type='int',
                      help="fade millis for color commands")

    parser.add_option('-t', '--delay', dest='delay_millis', default=500,type='int',
                      help="millis between commands like blink, random, etc.")

    parser.add_option('--debug', action="store_true", dest='debug' )
    parser.add_option('--on',    action="store_const",dest='rgb',const="#FFFFFF")
    parser.add_option('--off',   action="store_const",dest='rgb',const="#000000")
    parser.add_option('--red',   action="store_const",dest='rgb',const="#FF0000")
    parser.add_option('--green', action="store_const",dest='rgb',const="#00FF00")
    parser.add_option('--blue',  action="store_const",dest='rgb',const="#0000FF")

    (options, args) = parser.parse_args()

    rgbstr = options.rgb
    fade_millis = options.fade_millis
    ledn = options.ledn
    rgb = parse_color_string( rgbstr )
    debug_rw = options.debug

    #print "rgbval:%s millis:%i ledn:%i " % (repr(rgb),fade_millis,ledn)

    #
    blink1 = Blink1()

    if blink1.dev == None :
        print("no blink1 found")

    # blink command (takes an argument of number of blinks)
    if options.blink :
        if not rgb : rgb = (255,255,255)
        for i in range(0,options.blink):
            blink1.fade_to_rgbn( fade_millis, rgb[0],rgb[1],rgb[2], ledn)
            time.sleep( options.delay_millis / 1000.0 )
            blink1.fade_to_rgbn( fade_millis, 0,0,0, ledn)
            time.sleep( options.delay_millis / 1000.0 )
            
    elif options.play :
        play = map(int, options.play.split(',')) # convert to int list
        #print "play: "+repr(options.play) + ','+repr(play)
        play.extend( [0] * (4 - len(play)) )  # make list fixed size, seems dumb
        blink1.playloop( play[0], play[1], play[2], play[3] )

    elif options.cmd == "version":
        print "version: "+ blink1.get_version()

    elif options.cmd == "demo" :
        demo(blink1)
        
    elif options.cmd == None and rgb :
        blink1.fade_to_rgbn( fade_millis, rgb[0],rgb[1],rgb[2], ledn)

    else:
        parser.print_help()


