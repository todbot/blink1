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
import re

import usb 

debug_rw = False

class Blink1:

    def __init__(self):
        return self.find()
    
    def find(self):
        self.dev = usb.core.find(idVendor=0x27b8, idProduct=0x01ed)
        if( self.dev == None ): return None

    def notfound(self):
        return None  # fixme what to do here

    """
    """
    def write(self,buf):
        if debug_rw : print "blink1write: " + ",".join( '0x%02x' % v for v in buf )
        if( self.dev == None ): return self.notfound()
        bmRequestTypeOut = usb.util.build_request_type(usb.util.CTRL_OUT, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        self.dev.ctrl_transfer(bmRequestTypeOut, 0x09, (3 << 8) | 0x01, 0, buf)
        
    def read(self):
        bmRequestTypeIn = usb.util.build_request_type(usb.util.CTRL_IN, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_INTERFACE)
        buf = self.dev.ctrl_transfer(bmRequestTypeIn, 0x01, (3 << 8) | 0x01, 0, 8)
        if debug_rw : print "blink1read:  " + ",".join( '0x%02x' % v for v in buf )
        return buf

    """
    Command blink(1) to fade to RGB color

    """
    def fade_to_rgb(self, red,green,blue, fadeMillis, ledn):
        #action = 0x6E # ='n' (set rgb now)
        #action = 0x63 # ='c' (fade to rgb)
        action = ord('c')
        fadeMillis = fadeMillis/10
        th = (fadeMillis & 0xff00) >> 8
        tl = fadeMillis & 0x00ff
        buf = [0x01, action, red,green,blue, th,tl, ledn]
        return self.write(buf)

    """
    """
    def playloop(self, play,startpos,endpos,count):
        buf = [0x01, ord('p'), play, startpos, endpos, count]
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
def demo(blin1):

    print "blink1 version: "+ blink1.get_version()
        
    democolors = [ [255,  0,  0],  # red
                   [  0,255,  0],  # grn
                   [  0,  0,255],  # blu
                   [255,255,  0],  # yellow
                   [  0,255,255],  # cyan
                   [255,  0,255],  # magenta
                   [  0,  0,  0],  # off
                   ]
        
    fademillis = 100
    
    for rgb in democolors:
        r = rgb[0]
        g = rgb[1]
        b = rgb[2]
        print "fading to %3i,%3i,%3i" % (r,g,b)
        blink1.fade_to_rgb( r,g,b, fademillis, 0 )
        time.sleep(0.5)
        
        blink1.fade_to_rgb(   0,0,0, 1000, 0 )


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

    parser.add_option('--rgb', dest='rgb', default='',
                      help="the RGB color to use")

    parser.add_option('-l', '--led', dest='ledn', default=0, type='int')
    parser.add_option('-m', '--millis', dest='fade_millis', default=300, type='int')
    parser.add_option('-t', '--delay', dest='delay_millis', default=500,type='int')
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
            blink1.fade_to_rgb( rgb[0],rgb[1],rgb[2], fade_millis, ledn)
            time.sleep( options.delay_millis / 1000.0 )
            blink1.fade_to_rgb( 0,0,0, fade_millis, ledn)
            time.sleep( options.delay_millis / 1000.0 )
            
    elif options.cmd == "version":
        print "version: "+ blink1.get_version()

    elif options.cmd == "demo" :
        demo(blink1)
        
    elif options.cmd == None and rgb :
        blink1.fade_to_rgb( rgb[0],rgb[1],rgb[2], fade_millis, ledn)

    else:
        parser.print_help()

        

"""
    blink1 = Blink1()

    if( blink1.dev == None ):
        print("no blink1 found")
    
    demo(blink1)
   
""" 

