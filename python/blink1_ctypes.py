#!/usr/bin/env python

"""

blink1_ctypes.py -- blink(1) Python library

Uses ctypes wrapper around blink1-lib C library

Make sure you have the blink1-lib shared library in the same directory
as blink1_ctypes.py or in your LD_LIBRARY_PATH

Based on Stephen Youndt's script on how to wrap the C library

2013, Tod E. Kurt, http://thingm.com/

"""

import time
import string
import re

from ctypes import *
from ctypes.util import find_library
import inspect, os
import glob

# Find the library 
localpath = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
libname = find_library("blink1")
if libname is None:
    libname = find_library("Blink1")
if libname is None:
    libname = find_library("blink1-lib")
if libname is None:
    libname = glob.glob(os.path.join(localpath, '[Bb]link1-lib.so'))[-1]
if libname is None:
    libname = glob.glob(os.path.join(localpath, 'blink1-lib.dll'))[-1]
if libname is None:
    libname = glob.glob(os.path.join(localpath, 'lib[Bb]link1*'))[-1]

# If we found the library, load it
assert libname is not None
libblink1 = CDLL(libname)

enumerate = libblink1.blink1_enumerate
enumerate.restype = c_int
enumerateByVidPid = libblink1.blink1_enumerateByVidPid
enumerateByVidPid.restype = c_int
enumerateByVidPid.argtypes = [c_int, c_int]
getCachedPath = libblink1.blink1_getCachedPath
getCachedPath.restype = c_char_p
getCachedPath.argtypes = [c_int]
getCachedSerial = libblink1.blink1_getCachedSerial
getCachedSerial.restype = c_wchar_p
getCachedSerial.argtypes = [c_int]
getCachedCount = libblink1.blink1_getCachedCount
getCachedCount.restype = c_int
open = libblink1.blink1_open
open.restype = c_void_p
openByPath = libblink1.blink1_openByPath
openByPath.restype = c_void_p
openByPath.argtypes = [c_char_p]
openBySerial = libblink1.blink1_openBySerial
openBySerial.restype = c_void_p
openBySerial.argtypes = [c_wchar_p]
openById = libblink1.blink1_openById
openById.restype = c_void_p
openById.argtypes = [c_int]
close = libblink1.blink1_close
close.argtypes = [c_void_p]
#
getVersion = libblink1.blink1_getVersion
getVersion.restype = c_int
getVersion.argtypes = [c_void_p]
fadeToRGB = libblink1.blink1_fadeToRGB
fadeToRGB.restype = c_int
fadeToRGB.argtypes = [c_void_p, c_ushort, c_ubyte, c_ubyte, c_ubyte]
fadeToRGBN = libblink1.blink1_fadeToRGBN
fadeToRGBN.restype = c_int
fadeToRGBN.argtypes = [c_void_p, c_ushort, c_ubyte, c_ubyte, c_ubyte, c_ubyte]
setRGB = libblink1.blink1_setRGB
setRGB.restype = c_int
setRGB.argtypes = [c_void_p, c_ubyte, c_ubyte, c_ubyte]
serverdown = libblink1.blink1_serverdown
serverdown.restype = c_int
serverdown.argtypes = [c_void_p, c_ubyte, c_ushort]
play = libblink1.blink1_play
play.restype = c_int
play.argtypes = [c_void_p, c_ubyte, c_ubyte]
writePatternLine = libblink1.blink1_writePatternLine
writePatternLine.restype = c_int
writePatternLine.argtypes = [c_void_p, c_ushort, c_ubyte, c_ubyte, c_ubyte, c_ubyte]
readPatternLine = libblink1.blink1_readPatternLine
readPatternLine.restype = c_int
readPatternLine.argtypes = [c_void_p, c_void_p,c_void_p,c_void_p,c_void_p,c_void_p]
enableDegamma = libblink1.blink1_enableDegamma
disableDegamma = libblink1.blink1_disableDegamma

#################################################################################

debug_rw = False

class Blink1:
    
    def __init__(self):
        self.dev = None
        self.open()
    
    def find(self):
        return self.open()
        
    def open(self):
        self.dev = open()
        
    def open_by_id(self,id):
        self.dev = openById(id)

    def close(self):
        if self.dev != None:
            close(self.dev)
            
    def notfound(self):
        return None  # fixme what to do here


    """
    Command blink(1) to fade to RGB color

    """
    def fade_to_rgbn(self, fadeMillis, red,green,blue, ledn):
        #print("rgb:"+red+","+green+","+blue)
        fadeToRGBN( self.dev, fadeMillis, red,green,blue, ledn)

    """
    Command blink(1) to fade to RGB color

    """
    def fade_to_rgb(self, fadeMillis, red,green,blue):
        return self.fade_to_rgbn( fadeMillis, red,green,blue, 0)

    """
    """
    def playloop(self, play,startpos,endpos,count):
        playloop(self.dev, play, startpos,endpos, count)

    """
    """
    def play(self, play,startpos):
        playloop(self.dev, play, startpos,endpos, count)

    """
    Get blink(1) firmware version

    """
    def get_version(self):
        return str(getVersion(self.dev))

    

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
        
    fademillis = 100
    
    for rgb in democolors:
        r = rgb[0]
        g = rgb[1]
        b = rgb[2]
        print "fading to %3i,%3i,%3i" % (r,g,b)
        blink1.fade_to_rgbn( fademillis, r,g,b, 0 )
        time.sleep(0.5)
        
    blink1.fade_to_rgbn( 1000,  0,0,0,  0 )


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


