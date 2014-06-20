#!/usr/bin/env python

import sys
import time
import re
import sys
import uuid

debugimport=False
use_pyusb=False
try:
    from blink1_pyusb import Blink1 as Blink1_pyusb
    use_pyusb = True
    #sys.modules['Blink1'] = blink1_pyusb
    if debugimport: print "using blink1_pyusb"
except ImportError:
    try: 
        from blink1_ctypes import Blink1 as Blink1_ctypes
        #sys.modules['Blink1'] = blink1_ctypes
        if debugimport: print "using blink1_ctypes" 
    except ImportError:
        print "couldn't load blink1_pyusb or blink1_ctypes"
        sys.exit(1)


hostid = uuid.uuid4().hex[:8]

class Blink1:
    '''
    Object wrapper class.
    This a wrapper for objects. It is initialiesed with the object to wrap
    and then proxies the unhandled getattribute methods to it.
    Other classes are to inherit from it.
    '''
    def __init__(self):
        '''
        Wrapper constructor.
        '''
        # wrap the object
        if use_pyusb : 
            blink1 = Blink1_pyusb()
        else : 
            blink1 = Blink1_ctypes()
        self._wrapped_obj = blink1
            
    def __getattr__(self, attr):
        # see if this object has attr
        # NOTE do not use hasattr, it goes into
        # infinite recurrsion
        if attr in self.__dict__:
            # this object has it
            return getattr(self, attr)
        # proxy to the wrapped object
        try : 
            return getattr(self._wrapped_obj, attr)
        except Exception:
            print "****** error!"
            return None

# FIXME: can't overload methods?
#    def fade_to_rgb(self, millis, colorstr):
#        rgb = Blink1.parse_color_string(colorstr)
#        self._wrapped_obj.fade_to_rgb(millis, rgb[0], rgb[1], rgb[2])

    def get_hostid(self):  # FIXME
        return hostid

    def get_blink1id(self):
        return self.get_hostid() + self.get_serialnumber()


    @classmethod
    def parse_color_string(cls,rgbstr):
        '''
        '''
        rgbstr = rgbstr.lower()
        rgb = None
        # match hex color code "#FFcc00"
        m = re.search(r"#([0-9a-f]{6})", rgbstr)
        if m:
            rgb = tuple(ord(c) for c in m.group(1).decode('hex'))
        else:
            # match color triplets like "255,0,0" and "0xff,0xcc,0x33"
            m = re.search(r"(0x[\da-f]+|\d+),(0x[\da-f]+|\d+),(0x[\da-f]+|\d+)",
                          rgbstr)
            if m:
                rgb = tuple(int(c,0) for c in m.groups())

        return rgb


class Blink1Pattern:
    def __init__(self):
        self.repeats = 0
        self.colors = []
        self.times = []
        
    def __repr__(self):
        return "{ 'repeats': "+ repr(self.repeats) +", 'colors': "+repr(self.colors)+", 'times':"+repr(self.times) +" }"

    def __str__(self):
        #strtimes = "".join( str(n) for n in self.times )
        return "{ 'repeats': "+ str(self.repeats) +", 'colors': "+str(self.colors)+", 'times': [" + ", ".join( str(n) for n in self.times ) +"] }"

    @classmethod
    def parse_pattern_string(cls, pattstr):
        '''
        parse color patterns in the format: '3, #ff00ff, 1.5 ,#000000, 1.0'
        '''
        print "parse_pattern_string:"+pattstr
        vals = pattstr.split(',')
        if( len(vals) % 2 == 0 ) : # even is bad, must be odd
            print "bad patternstr: "+pattstr
        else:
            patt = Blink1Pattern()
            patt.repeats = int(vals[0]) # 
            # every other element from pos 2
            # every other element from pos 2
            patt.colors  = map( Blink1.parse_color_string, vals[1::2]) 
            patt.times   = [float(m) for m in vals[2::2]] 
            return patt


def demo(blink1):
    '''
    '''
    print "blink1 version: "+ blink1.get_version()
        
    democolors = [ (255,  0,  0),  # red
                   (  0,255,  0),  # grn
                   (  0,  0,255),  # blu
                   (255,255,  0),  # yellow
                   (  0,255,255),  # cyan
                   (255,  0,255),  # magenta
                   (  0,  0,  0),  # off
                   ]

    demo_millis = 200

    for rgb in democolors:
        (r,g,b) = rgb

        print "fading to %3i,%3i,%3i" % (r,g,b)
        blink1.fade_to_rgbn( demo_millis/2, r,g,b, 0 )
        time.sleep( demo_millis/1000.0 ) 
        blink1.fade_to_rgbn( demo_millis/2, 0,0,0, 0 )
        time.sleep( demo_millis/1000.0 ) 


def main():
    '''
    '''
    from optparse import OptionParser
    
    parser = OptionParser()
    parser.add_option('--demo',    
                      action='store_const', dest='cmd',const='demo',
                      help='run simple demo')

    parser.add_option('--version', 
                      action='store_const', dest='cmd',const='version',
                      help='return firmware version')

    parser.add_option('--hostid', 
                      action='store_const', dest='cmd',const='hostid',
                      help='return hostid')

    parser.add_option('--blink', 
                      dest='blink',default=0, type='int',
                      help='blink specified number of times')

    parser.add_option('--play', 
                      dest='play',default=0, type='string',
                      help='play built-in light sequence')

    parser.add_option('--patt', 
                      dest='patt',default=0, type='string',
                      help='play specified color pattern')

    parser.add_option('--rgb', default='',
                      dest='rgb', 
                      help="the RGB color to use")

    parser.add_option('-l', '--led',  default=0, type='int',
                      dest='ledn',
                      help="which LED to use (default=both)")

    parser.add_option('-m', '--millis', default=300, type='int',
                      dest='fade_millis', 
                      help="fade millis for color commands")

    parser.add_option('-t', '--delay', default=500, type='int',
                      dest='delay_millis',
                      help="millis between commands like blink, random, etc.")

    parser.add_option('--debug',action="store_true", dest='debug' )
    parser.add_option('--on',   action="store_const",dest='rgb',const="#FFFFFF")
    parser.add_option('--off',  action="store_const",dest='rgb',const="#000000")
    parser.add_option('--red',  action="store_const",dest='rgb',const="#FF0000")
    parser.add_option('--green',action="store_const",dest='rgb',const="#00FF00")
    parser.add_option('--blue', action="store_const",dest='rgb',const="#0000FF")

    (options, args) = parser.parse_args()

    rgbstr = options.rgb
    fade_millis = options.fade_millis
    ledn = options.ledn
    rgb = Blink1.parse_color_string( rgbstr )
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
        play = map(int, options.play.split(',')) # convert str list to int list
        #print "play: "+repr(options.play) + ','+repr(play)
        play.extend( [0] * (4 - len(play)) )  # make list fixed size, seems dumb
        blink1.playloop( play[0], play[1], play[2], play[3] )

    elif options.patt : 
        blink1patt = Blink1Pattern.parse_pattern_string(options.patt)
        print "playing pattern: "+ str(blink1patt)
        for i in range(blink1patt.repeats):
            for j in range(len(blink1patt.colors)):
                color = blink1patt.colors[j]
                millis = int( blink1patt.times[j] * 1000 )
                print "color: "+str(color) +", millis: "+ str(millis)
                blink1.fade_to_rgb( millis/2, color[0], color[1], color[2])
                time.sleep( millis / 1000.0 )

    elif options.cmd == 'version':
        print "version: "+ blink1.get_version()

    elif options.cmd == 'hostid':
        print "hostid: "+ blink1.get_hostid()

    elif options.cmd == 'demo' :
        demo(blink1)
        
    elif options.cmd == None and rgb :
        print "fading to #%02x%02x%02x" % (rgb) + " in %d msec" % fade_millis
        blink1.fade_to_rgbn( fade_millis, rgb[0],rgb[1],rgb[2], ledn)

    else:
        parser.print_help()


if __name__ == "__main__":
    sys.exit(main())
