import unittest
import math
import mock
import time
from blink1.kelvin import kelvin_to_rgb
from blink1.blink1 import blink1


class TestGamma(unittest.TestCase):

    def test_red_6000(self):
        r, g, b = kelvin_to_rgb(6000)
        self.assertEquals(r, 255)

    def test_blue_6700(self):
        r, g, b = kelvin_to_rgb(6700)
        self.assertEquals(b, 255)

    def test_range(self):
        """Verify that it is possible to produce the entire color range"""

        for k in range(10, 12000, 400):
            r,g,b = kelvin_to_rgb(k)
            self.assertIsInstance(r, int)
            self.assertIsInstance(g, int)
            self.assertIsInstance(b, int)

            with blink1(white_point=(r,g,b)) as b1:
                b1.fade_to_color(0, 'white')
                time.sleep(0.05)


if __name__ == '__main__':
    unittest.main()