import unittest
import time

from blink1.blink1 import blink1
from blink1.kelvin import COLOR_TEMPERATURES


class TestBlink1ContextManager(unittest.TestCase):
    def test_cm(self):
        with blink1() as b1:
            b1.fade_to_color(0, "teal")

    def test_cm_closed(self):
        with blink1() as b1:
            b1.close()
            b1.fade_to_color(0, "teal")

    def test_cm_with_gamma(self):
        with blink1(gamma=(.5, .5, .5)) as b1:
            b1.fade_to_color(0, "teal")

    def test_cm_with_white_point(self):
        with blink1(white_point=(255, 255, 255)) as b1:
            b1.fade_to_color(0, "white")
            time.sleep(0.1)

    def test_cm_with_white_point(self):
        with blink1(white_point=COLOR_TEMPERATURES['candle']) as b1:
            b1.fade_to_color(0, "white")
            time.sleep(0.1)

    def test_cm_with_white_point(self):
        with blink1(white_point='blue-sky') as b1:
            b1.fade_to_color(0, "white")
            time.sleep(0.1)



if __name__ == '__main__':
    unittest.main()