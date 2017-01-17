"""
blink1.py -- blink(1) Python library using PyUSB

Based on blink1hid-demo.py by Aaron Blondeau and the reference Python Plink(1)
library by Tod E. Kurt, http://thingm.com/

Uses "PyUSB 1.0" to do direct USB HID commands
See: https://github.com/walac/pyusb

All platforms:
 % sudo pip install blink1

Windows Only
 You will need to manually install libusb-win32
"""
import logging
import time
import sys
from contextlib import contextmanager
import usb
import webcolors

from .kelvin import kelvin_to_rgb, COLOR_TEMPERATURES

class BlinkConnectionFailed(RuntimeError):
    """Raised when we cannot connect to a Blink(1)
    """

class InvalidColor(ValueError):
    """Raised when the user requests an implausible colour
    """


log = logging.getLogger(__name__)

DEFAULT_GAMMA = (2, 2, 2)
DEFAULT_WHITE_POINT = (255, 255, 255)

REPORT_ID = 0x01
VENDOR_ID = 0x27b8
PRODUCT_ID = 0x01ed

class ColorCorrect(object):
    """Apply a gamma correction to any selected RGB color, see:
    http://en.wikipedia.org/wiki/Gamma_correction
    """
    def __init__(self, gamma, white_point):
        """
        :param gamma: Tuple of r,g,b gamma values
        :param white_point: White point expressed as (r,g,b), integer color temperature (in Kelvin) or a string value.

        All gamma values should be 0 > x >= 1
        """
        self.gamma = gamma

        if isinstance(white_point, str):
            kelvin = COLOR_TEMPERATURES[white_point]
            self.white_point = kelvin_to_rgb(kelvin)
        elif isinstance(white_point,(int,float)):
            self.white_point = kelvin_to_rgb(white_point)
        else:
            self.white_point = white_point

    @staticmethod
    def gamma_correct(gamma, white, luminance):
        return round(white * (luminance / 255) ** gamma)

    def __call__(self, r, g, b):
        color = [r,g,b]
        return tuple(self.gamma_correct(g, w, l) for (g, w, l) in zip(self.gamma, self.white_point, color) )


class Blink1:
    """Light controller class, sends messages to the blink(1) and blink(1) mk2 via USB HID.
    """
    def __init__(self, gamma=None, white_point=None):
        """
        :param gamma: Triple of gammas for each channel e.g. (2, 2, 2)
        """
        self.cc = ColorCorrect(
            gamma=(gamma or DEFAULT_GAMMA),
            white_point=(white_point or DEFAULT_WHITE_POINT)
        )

        self.dev = self.find()
        if not self.dev:
            raise BlinkConnectionFailed("Could not find an attached Blink(1)")

    def close(self):
        self.dev = None

    @staticmethod
    def find():
        dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
        if dev == None:
            return None

        if not sys.platform.startswith('win'):
            if ( dev.is_kernel_driver_active(0) ):
                try:
                    dev.detach_kernel_driver(0)
                except usb.core.USBError as e:
                    sys.exit("Could not detatch kernel driver: %s" % str(e))
        return dev


    def notfound(self):
        return None  # fixme what to do here

    def write(self, buf):
        """
        Write command to blink(1)
        Send USB Feature Report 0x01 to blink(1) with 8-byte payload
        Note: arg 'buf' must be 8 bytes or bad things happen
        """
        log.debug("blink1write:" + ",".join('0x%02x' % v for v in buf))
        if ( self.dev == None ): return self.notfound()
        bmRequestTypeOut = usb.util.build_request_type(usb.util.CTRL_OUT, usb.util.CTRL_TYPE_CLASS,
                                                       usb.util.CTRL_RECIPIENT_INTERFACE)
        self.dev.ctrl_transfer(bmRequestTypeOut,
                               0x09,  # == HID set_report
                               (3 << 8) | REPORT_ID,  # (3==HID feat.report)
                               0,
                               buf)

    def read(self):
        """
        Read command result from blink(1)
        Receive USB Feature Report 0x01 from blink(1) with 8-byte payload
        Note: buf must be 8 bytes or bad things happen
        """
        bmRequestTypeIn = usb.util.build_request_type(usb.util.CTRL_IN, usb.util.CTRL_TYPE_CLASS,
                                                      usb.util.CTRL_RECIPIENT_INTERFACE)
        buf = self.dev.ctrl_transfer(bmRequestTypeIn,
                                     0x01,  # == HID get_report
                                     (3 << 8) | REPORT_ID,
                                     0,
                                     8)  # == number of bytes to read
        log.debug("blink1read: " + ",".join('0x%02x' % v for v in buf))
        return buf


    def fade_to_rgb_uncorrected(self, fade_milliseconds, red, green, blue, led_number=0):
        """
        Command blink(1) to fade to RGB color, no color correction applied.
        """
        action = ord('c')
        fade_time = int(fade_milliseconds / 10)
        th = (fade_time & 0xff00) >> 8
        tl = fade_time & 0x00ff
        buf = [REPORT_ID, action, red, green, blue, th, tl, led_number]
        return self.write(buf)

    def fade_to_rgb(self,fade_milliseconds, red, green, blue, led_number=0):
        r, g, b = self.cc(red, green, blue)
        return self.fade_to_rgb_uncorrected(fade_milliseconds, r, g, b, led_number=led_number)

    @staticmethod
    def color_to_rgb(color):
        if isinstance(color, tuple):
            return color
        if color.startswith('#'):
            try:
                return webcolors.hex_to_rgb(color)
            except ValueError:
                raise InvalidColor(color)

        try:
            return webcolors.name_to_rgb(color)
        except ValueError:
            raise InvalidColor(color)


    def fade_to_color(self, fade_milliseconds, color, led_number=0):
        """
        Fade the light to a known colour in a
        :param fade_milliseconds: Duration of the fade in milliseconds
        :param color: Named color to fade to
        :return: None
        """
        red, green, blue = self.color_to_rgb(color)

        return self.fade_to_rgb(fade_milliseconds, red, green, blue, led_number=led_number)

    def off(self):
        """Switch the blink(1) off instantly
        """
        self.fade_to_color(0, 'black')

    def get_version(self):
        """Get blink(1) firmware version
        """
        if ( self.dev == None ): return ''
        buf = [0x01, ord('v'), 0, 0, 0, 0, 0, 0]
        self.write(buf)
        time.sleep(.05)
        version_raw = self.read()
        version = (version_raw[3] - ord('0')) * 100 + (version_raw[4] - ord('0'))
        return str(version)

    def get_serial_number(self):
        """Get blink(1) serial number
        """
        return usb.util.get_string(self.dev, 3)


@contextmanager
def blink1(switch_off=True, gamma=None, white_point=None):
    """Context manager which automatically shuts down the Blink(1)
    after use.
    """
    b1 = Blink1(gamma=gamma, white_point=white_point)
    yield b1
    if switch_off:
        b1.off()
    b1.close()

