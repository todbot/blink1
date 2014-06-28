import time
import random
import socket
import logging
import zmq
from .flash import mk_flash_msg, COLOURS
from .config import UPSTREAM_URL


log = logging.getLogger(__name__)


context = zmq.Context()


def counter():
    i = 0
    while True:
        yield i
        i += 1


def run():
    socket = context.socket(zmq.PUB)
    socket.connect(UPSTREAM_URL)
    log.info("Publisher connected to %s" % UPSTREAM_URL)

    for i in counter():
        times = random.randint(1, 9)
        duration = random.uniform(0.2, 0.8)
        colour = random.choice(COLOURS)
        msg = mk_flash_msg(times=times, duration=duration, colour=colour)
        print("%i -> %s" % (i, repr(msg)))
        socket.send_string(msg)
        time.sleep(10)

def main():
    logging.basicConfig()
    logging.getLogger("").setLevel(logging.INFO)
    run()

if __name__ == '__main__':
    main()
