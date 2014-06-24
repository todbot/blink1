import time
from blink1.blink1 import blink1

with blink1() as b1:
    b1.fade_to_color(100, 'navy')
    time.sleep(10)