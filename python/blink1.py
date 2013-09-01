#!/usr/bin/env python

import sys
import time
import re
import sys

debugimport=False
try:
    from blink1_pyusb import Blink1
    #sys.modules['Blink1'] = blink1_pyusb
    if debugimport: print "using blink1_pyusb"
except ImportError:
    try: 
        from blink1_ctypes import Blink1
        #sys.modules['Blink1'] = blink1_ctypes
        if debugimport: print "using blink1_ctypes" 
    except ImportError:
        print "couldn't load blink1_pyusb or blink1_ctypes"
        sys.exit(1)


"""
"""
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

"""
"""
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


def main():

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


if __name__ == "__main__":
    sys.exit(main())
