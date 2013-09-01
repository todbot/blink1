"""
blink1_ctypes_tst -- simple demo of blink1_ctypes library

You can also just run blink1_pyusb.py as a blink1-tool replacement

"""

from blink1_ctypes import Blink1
import time


blink1 = Blink1()

if( blink1.dev == None ):
    print("no blink1 found")

blink1.fade_to_rgb( 1000,  255,255,255 )

time.sleep(0.5)
        
blink1.fade_to_rgb( 1000,  0,0,0 )
        


