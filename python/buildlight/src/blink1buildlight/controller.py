"""
Build light controller
"""
import zmq
import json
import time
import itertools
import logging
from .discover import discover
from contextlib import contextmanager
from datetime import timedelta

import webcolors


log = logging.getLogger(__name__)

COLOURS = list(webcolors.css3_names_to_hex.keys())
DEFAULT_THROB = ["red", "yellow", "pink", "green", "orange", "purple", "blue", "black"]
DEFAULT_CHANNEL = 'buildlight'

class BuildLightRequest(object):
    """
    Class for requesting build-light actions.
    """
    def __init__(self, socket, channel=DEFAULT_CHANNEL):
        self.socket = socket
        self.counter = itertools.count()
        self.channel = channel

    def send_message(self, command, payload):
        event_no = next(self.counter)
        payload["event_no"] = event_no
        msg = '%s\n%s\n%s' % (
            self.channel,
            command,
            json.dumps(payload)
        )
        log.info("%i -> %s" % (event_no, repr(msg)))
        self.socket.send_string(msg)


    def flash(self, times, duration=1, colour="white"):
        payload = {'times': times, 'duration': duration, 'colour': colour}
        self.send_message("flash", payload)

    def throb(self, freq=1, fade=1, colours=DEFAULT_THROB):
        payload = {'freq':freq, 'fade':fade, 'colours':colours}
        self.send_message("throb", payload)

class Receiver(object):
    """
    Receives build light requests, controls a build-light.
    """

    def __init__(self, b1, loop):
        self.b1 = b1
        self.loop = loop

        self.set_throb(1, 1 , DEFAULT_THROB)
        self.throb_enabled = True

    def decode_msg(self, msg):
        message_channel, message_type, encoded_payload = msg.decode('utf-8').split('\n')
        return message_channel, message_type, json.loads(encoded_payload)

    def recieve(self, msg):
        for m in msg:
            msg_channel, msg_type, msg_args = self.decode_msg(m)
            event_no = msg_args.pop('event_no')

            if msg_type == 'flash':
                self.flash_led(**msg_args)
            elif msg_type == 'throb':
                self.set_throb(**msg_args)
            else:
                raise ValueError("Unknown command %s" % msg_type)



    def set_throb(self, freq, fade, colours):
        log.info("Setting throb pattern to %r f=%.2f Hz" % (colours, freq))
        self.throb_freq = freq
        self.throb_fade = fade
        self.throb_colours = colours

    def enable_throb(self):
        log.info("Enabling throb")
        self.throb_enabled = True
        self.loop.add_callback(self.throbber)

    def disable_throb(self):
        if self.throb_enabled:
            log.info("Disabling throb")
            self.throb_enabled = False

    def throbber(self, throb_count=0):
        if self.throb_enabled:
            throb_sequence_length = len(self.throb_colours)
            throb_index = throb_count % throb_sequence_length
            colour = self.throb_colours[throb_index]
            log.info("Throbbing: %s (%i/%i)" % (
                colour,
                throb_index,
                throb_sequence_length
            ))

            self.b1.fade_to_color(self.throb_fade * 1000, colour)

            callback = lambda : self.throbber(throb_count + 1)
            self.loop.add_timeout(timedelta(seconds=1/self.throb_freq), callback)


    def flash_led(self, times=1, total_times=None, duration=1.0, colour='white', on=True, fade=0):
        self.disable_throb()
        log.info("Flashing {colour}/{state} for {duration}s {times}/{total_times}".format(
            state=on and 'on' or 'off',
            **locals()))

        if on:
            self.b1.fade_to_color(fade, colour)
        else:
            self.b1.fade_to_color(fade, 'black')

        total_times = total_times or times

        if on == False:
            remaining_callback = lambda: self.flash_led(times - 1, total_times=total_times, duration=duration,
                                                   colour=colour)
        else:
            remaining_callback = lambda: self.flash_led(times, total_times=total_times, duration=duration,
                                                   colour=colour,
                                                   on=False)

        if times > 0 or on:
            self.loop.add_timeout(timedelta(seconds=duration), remaining_callback)
        else:
            self.enable_throb()

@contextmanager
def get_build_light_proxy(context, url=None):
    """
    A handy context manager to make it super easy to discover and connect to a build-light proxy.

    If the URL is not provided it will attempt to discover a server on the local network.

    :param url: URL for the bl_proxy - tcp://server.foocorp:1234
    """
    url = url or discover()['upstream']
    socket = context.socket(zmq.PUB)
    socket.connect(url)
    log.info("Build light requester connected to %s" % url)
    blr = BuildLightRequest(socket)
    time.sleep(0.1)
    yield blr
    socket.close()

