"""

blink1_ctypes.py -- blink(1) Python library

Uses ctypes wrapper around blink1-lib C library (which in turn wraps HIDAPI)

Make sure you have the blink1-lib shared library in the same directory
as blink1_ctypes.py or in your LD_LIBRARY_PATH

Based on Stephen Youndt's script on how to wrap the C library

2013, Tod E. Kurt, http://thingm.com/

"""

import time

from ctypes import *
from ctypes.util import find_library
import inspect, os
import glob

# Find the blink1-lib C library 
localpath = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
libname = find_library("blink1")
if libname is None:
    libname = find_library("Blink1")
if libname is None:
    libname = find_library("blink1-lib")
if libname is None:
    pathlist = glob.glob(os.path.join(localpath, '[Bb]link1-lib.so')) # unix
    if pathlist: libname = pathlist[-1]
if libname is None:
    pathlist = glob.glob(os.path.join(localpath, 'blink1-lib.dll')) # windows
    if pathlist: libname = pathlist[-1]
if libname is None:
    pathlist = glob.glob(os.path.join(localpath, 'lib[Bb]link1*')) # mac
    if pathlist: libname = pathlist[-1]

# If we found the library, load it
if not libname:
    raise ImportError("no blink1-lib shared library found")

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
getCachedSerial.restype = c_char_p
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
openBySerial.argtypes = [c_char_p]
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

    def enumerate(self):
        enumerate()
        
    def open(self):
        self.close()
        self.dev = open()
        
    def open_by_id(self,id):
        self.dev = openById(id)

    def close(self):
        if self.dev != None:
            close(self.dev)
            self.dev = None

    def notfound(self):
        return None  # fixme what to do here


    def fade_to_rgbn(self, fade_millis, red,green,blue, ledn):
        """
        Command blink(1) to fade to RGB color
        
        """
        return fadeToRGBN( self.dev, fade_millis, red,green,blue, ledn)

    def fade_to_rgb(self, fade_millis, red,green,blue):
        """
        Command blink(1) to fade to RGB color
        
        """
        return self.fade_to_rgbn( fade_millis, red,green,blue, 0)


    def playloop(self, play,startpos,endpos,count):
        """
        """
        playloop(self.dev, play, startpos,endpos, count)

    def play(self, play,startpos):
        """
        """
        playloop(self.dev, play, startpos,endpos, count)

    def get_version(self):
        """
        Get blink(1) firmware version
        """
        return str(getVersion(self.dev))

    def get_serialnumber(self):
        '''
        Get blink(1) serial number
        '''

        sernum = getCachedSerial(0)
        if not sernum : sernum = '00000000'
        return sernum

    def get_serialnumbers(self):  # FIXME:
        seriallist = []
        for i in range(0, getCachedCount()):
            seriallist.append( getCachedSerial(i) )
        return seriallist
    


