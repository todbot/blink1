#/usr/bin/python
""" 
blink1.py -- Python interface to libblink1 through the magic of ctypes 

from blink1-ctypes import blink1
dev = blink1.open() 
blink1.setRGB(d, 255,0,0) # Red 
... 

Do not: 
from blink1 import * 
or you will polute your namespace with some very common names (i.e. File I/O) 

Make sure the "blink1-lib" shared library / DLL is in your path 
for more info see: 
https://getsatisfaction.com/thingm/topics/more_comprehensive_python_support

Thanks to Stephen Youndt for this
""" 

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
vid = libblink1.blink1_vid
vid.restype = c_int
pid = libblink1.blink1_pid
pid.restype = c_int
sortPaths = libblink1.blink1_sortPaths
sortSerials = libblink1.blink1_sortSerials
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
write = libblink1.blink1_write
write.restype = c_int
write.argtypes = [c_void_p, c_void_p, c_int]
read = libblink1.blink1_read
read.restype = c_int
read.argtypes = [c_void_p, c_void_p, c_int]
getSerialNumber = libblink1.blink1_getSerialNumber
getSerialNumber.restype = c_int
getSerialNumber.argtypes = [c_void_p, c_char_p]
getVersion = libblink1.blink1_getVersion
getVersion.restype = c_int
getVersion.argtypes = [c_void_p]
fadeToRGB = libblink1.blink1_fadeToRGB
fadeToRGB.restype = c_int
fadeToRGB.argtypes = [c_void_p, c_ushort, c_ubyte, c_ubyte, c_ubyte]
setRGB = libblink1.blink1_setRGB
setRGB.restype = c_int
setRGB.argtypes = [c_void_p, c_ubyte, c_ubyte, c_ubyte]
eeread = libblink1.blink1_eeread
eeread.restype = c_int
eeread.argtypes = [c_void_p, c_ushort, c_void_p]
eewrite = libblink1.blink1_eewrite
eewrite.restype = c_int
eewrite.argtypes = [c_void_p, c_ushort, c_void_p]
serialnumread = libblink1.blink1_serialnumread
serialnumread.restype = c_int
serialnumread.argtypes = [c_void_p, c_void_p]
serialnumwrite = libblink1.blink1_serialnumwrite
serialnumwrite.restype = c_int
serialnumwrite.argtypes = [c_void_p, c_void_p]
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
readPatternLine.argtypes = [c_void_p, c_void_p, c_void_p, c_void_p, c_void_p, c_void_p]
error_msg = libblink1.blink1_error_msg
error_msg.restype = c_char_p
error_msg.argtypes = [c_int]
enableDegamma = libblink1.blink1_enableDegamma
disableDegamma = libblink1.blink1_disableDegamma
degamma = libblink1.blink1_degamma
degamma.restype = c_int
degamma.argtypes = [c_int]
sleep = libblink1.blink1_sleep
sleep.argtypes = [c_ushort]

