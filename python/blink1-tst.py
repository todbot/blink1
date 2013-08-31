
from blink1 import Blink1
import time

blink1 = Blink1()

if( blink1.dev == None ):
    print("no blink1 found")

blink1.fade_to_rgb(   255,255,255, 1000, 0 )

time.sleep(0.5)
        
blink1.fade_to_rgb(   0,0,0, 1000, 0 )
        

