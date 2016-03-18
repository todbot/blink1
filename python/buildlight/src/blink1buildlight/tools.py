from .discover import discover
import click
import time
import random
import socket
import logging
import itertools
import zmq
from functools import wraps
from .controller import get_build_light_proxy, COLOURS
from .discover import discover


log = logging.getLogger(__name__)

context = zmq.Context()

def set_logging(fn):
    @wraps(fn)
    def wrapped(*args, **kwargs):
        logging.basicConfig()
        logging.getLogger("").setLevel(logging.INFO)
        return fn(*args, **kwargs)
    return wrapped


def validate_colour(c):
    if not c in COLOURS:
        raise ValueError("%s is not a valid colour" % c)

@set_logging
@click.command()
@click.option('--colours', default='red,green,blue', type=str)
@click.option('--freq', default=1, type=float)
@click.option('--url', default=None, type=str)
def set_throb(url=None, freq=1.0, colours="red,green,blue"):
    col_list = [c.strip() for c in colours.split(',')]

    for c in col_list:
        validate_colour(c)

    with get_build_light_proxy(context, url=url) as blr:
        time.sleep(0.1)
        blr.throb(freq=freq, colours=col_list)

@set_logging
@click.command()
@click.option('--colour', default='white', type=str)
@click.option('--times', default=3, type=int)
@click.option('--duration', default=1.0, type=float)
@click.option('--url', default=None, type=str)
def set_flash(url=None, colour="white", times=3, duration=1):
    validate_colour(colour)
    with get_build_light_proxy(context, url=url) as blr:
        time.sleep(0.1)
        blr.flash(
            colour=colour,
            duration=duration,
            times=times
        )