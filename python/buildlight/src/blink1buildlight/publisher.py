import time
import random
import socket
import logging
import itertools
import zmq
from .flash import mk_flash_msg, COLOURS
from .discover import discover

log = logging.getLogger(__name__)


context = zmq.Context()


def run():
    config = discover()
    upstream_url = config['upstream']

    socket = context.socket(zmq.PUB)
    socket.connect(upstream_url)
    log.info("Publisher connected to %s" % upstream_url)

    for i in itertools.count():
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
