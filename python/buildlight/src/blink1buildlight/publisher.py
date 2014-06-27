import time
import random

import zmq
from flasher.flash import mk_flash_msg, COLOURS


context = zmq.Context()


def counter():
    i = 0
    while True:
        yield i
        i += 1


def main():
    socket = context.socket(zmq.PUB)
    socket.bind("tcp://127.0.0.1:5000")

    for i in counter():
        times = random.randint(1, 9)
        duration = random.uniform(0.2, 0.8)
        colour = random.choice(COLOURS)
        msg = mk_flash_msg(times=times, duration=duration, colour=colour)
        print("%i -> %s" % (i, repr(msg)))
        socket.send_string(msg)
        time.sleep(10)


if __name__ == '__main__':
    main()
