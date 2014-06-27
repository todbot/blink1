import json
import logging
from datetime import timedelta

import webcolors


log = logging.getLogger(__name__)

COLOURS = webcolors.css3_names_to_hex.keys()

def mk_flash_msg(times, duration=1, colour='white'):
    payload = {'times': times, 'duration': duration, 'colour': colour}
    msg = 'flash\n%s' % json.dumps(payload)
    return msg


def decode_msg(msg):
    message_type, _, encoded_payload = msg.decode('utf-8').partition('\n')
    return message_type, json.loads(encoded_payload)


def flash_led(loop, b1, times=1, total_times=None, duration=1.0, colour='white', on=True):
    log.info("Flashing {colour}/{state} for {duration}s {times}/{total_times}".format(
        state=on and 'on' or 'off',
        **locals()))

    if on:
        b1.fade_to_color(50, colour)
    else:
        b1.fade_to_color(50, 'black')

    total_times = total_times or times

    if on == False:
        remaining_callback = lambda: flash_led(loop, b1, times - 1, total_times=total_times, duration=duration,
                                               colour=colour)
    else:
        remaining_callback = lambda: flash_led(loop, b1, times, total_times=total_times, duration=duration,
                                               colour=colour,
                                               on=False)

    if times > 0:
        loop.add_timeout(timedelta(seconds=duration), remaining_callback)
