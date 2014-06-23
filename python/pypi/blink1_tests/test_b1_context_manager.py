import unittest

from blink1.blink1 import blink1


class TestBlink1ContextManager(unittest.TestCase):
    def test_cm(self):
        with blink1() as b1:
            b1.fade_to_color(0, "teal")

    def test_cm_closed(self):
        with blink1() as b1:
            b1.close()
            b1.fade_to_color(0, "teal")


if __name__ == '__main__':
    unittest.main()