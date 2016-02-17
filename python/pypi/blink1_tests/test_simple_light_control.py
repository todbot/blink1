import unittest

import mock
from blink1.blink1 import Blink1, BlinkConnectionFailed, InvalidColor


class TestSimpleLightControl(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.b1 = Blink1()

    @classmethod
    def tearDownClass(cls):
        cls.b1.off()
        cls.b1.close()
        del cls.b1

    def testOn(self):
        self.b1.fade_to_color(1000, 'white')

    def testInvalidColor(self):
        with self.assertRaises(InvalidColor):
            self.b1.fade_to_color(1000, 'moomintrol')

    def testAlsoWhite(self):
        self.b1.fade_to_color(1000, (255,255,255))

    def testAWhiteShadeOfPale(self):
        self.b1.fade_to_color(1000, '#ffffff')

    def testAGreyerShadeOfPale(self):
        self.b1.fade_to_color(1000, '#eeeeee')

    def testAnImplausibleShadeOfWhite(self):
        with self.assertRaises(InvalidColor):
            self.b1.fade_to_color(1000, '#xxxxxx')

    def testOff(self):
        self.b1.off()

    def test_get_firmware_version(self):
        ver = self.b1.get_version()

    def test_get_serial_number(self):
        sn = self.b1.get_serial_number()


class TestFailedConnection(unittest.TestCase):
    def testCannotFind(self):
        with mock.patch('blink1.blink1.PRODUCT_ID', '0101'):
            with self.assertRaises(BlinkConnectionFailed):
                b1 = Blink1()


if __name__ == '__main__':
    unittest.main()