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


class BlinkConnectionFailed(RuntimeError):
    """Raised when we cannot connect to a Blink(1)
    """


log = logging.getLogger(__name__)

REPORT_ID = 0x01
VENDOR_ID = 0x27b8
PRODUCT_ID = 0x01ed

class Blink1:
    def __init__(self):
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

    def fade_to_rgbn(self, fade_milliseconds, red, green, blue, ledn):
        """Command blink(1) to fade to RGB color
        """
        action = ord('c')
        fade_time = int(fade_milliseconds / 10)
        th = (fade_time & 0xff00) >> 8
        tl = fade_time & 0x00ff
        buf = [REPORT_ID, action, red, green, blue, th, tl, ledn]
        return self.write(buf)

    def fade_to_rgb(self, fade_milliseconds, red, green, blue):
        """
        Command blink(1) to fade to RGB color
        """
        return self.fade_to_rgbn(fade_milliseconds, red, green, blue, 0)

    def fade_to_color(self, fade_milliseconds, color):
        """
        Fade the light to a known colour in a
        :param fade_milliseconds: Duration of the fade in milliseconds
        :param color: Named color to fade to
        :return: None
        """
        red, green, blue = webcolors.name_to_rgb(color)
        return self.fade_to_rgb(fade_milliseconds, red, green, blue)

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
        return usb.util.get_string(self.dev, 256, 3)


@contextmanager
def blink1(switch_off=True):
    """Context manager which automatically shuts down the Blink(1)
    after use.
    """
    b1 = Blink1()
    yield b1
    if switch_off:
        b1.off()
    b1.close()

