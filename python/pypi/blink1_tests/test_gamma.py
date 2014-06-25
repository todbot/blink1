import unittest
import math
import mock
from blink1.blink1 import Gamma


class TestGamma(unittest.TestCase):

    def testUnity(self):
        g = Gamma(1,1,1)
        self.assertEquals(
            g(255,255,255),
            (255,255,255)
        )

    def testFullPowerHalfGamma(self):
        expected = round(255 * (1 ** 0.5))
        g = Gamma(0.5,0.5,0.5)
        self.assertEquals(
            g(255,255,255),
            (expected,expected,expected)
        )

    def testHalfPowerHalfGamma(self):
        expected = round(255 * (127 / 255) ** 0.5)
        g = Gamma(0.5,0.5,0.5)
        self.assertEquals(
            g(127,127,127),
            (expected,expected,expected)
        )

if __name__ == '__main__':
    unittest.main()