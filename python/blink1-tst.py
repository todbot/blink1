
import blink1
import time

blink1 = blink1.Blink1()

if( blink1.dev == None ):
    print("no blink1 found")

blink1.fadeToRGB(   255,255,255, 1000, 0 )

time.sleep(0.5)
        
blink1.fadeToRGB(   0,0,0, 1000, 0 )
        

