from blink1.blink1 import Blink1
import time

b1 = Blink1()
b1.fade_to_rgb(1000, 64, 64, 64)
time.sleep(3)
b1.fade_to_rgb(1000, 255, 255, 255)