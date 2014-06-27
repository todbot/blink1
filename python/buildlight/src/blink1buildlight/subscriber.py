import logging

import zmq
from zmq.eventloop import ioloop
from flasher.flash import decode_msg, flash_led
from zmq.eventloop.zmqstream import ZMQStream
from blink1.blink1 import blink1

ioloop.install()
context = zmq.Context()


def main():
    socket = context.socket(zmq.SUB)
    socket.connect("tcp://127.0.0.1:5000")
    socket.setsockopt_string(zmq.SUBSCRIBE, u"flash")
    stream = ZMQStream(socket)

    loop = ioloop.IOLoop.instance()

    with blink1() as b1:
        def recieve(msg):
            for m in msg:
                msg_type, msg_args = decode_msg(m)
                flash_led(loop, b1, **msg_args)

        stream.on_recv(recieve)
        loop.start()

if __name__ == '__main__':
    logging.basicConfig()
    logging.getLogger("").setLevel(logging.INFO)
    main()
