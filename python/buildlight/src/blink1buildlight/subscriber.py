import logging
import zmq
from zmq.eventloop import ioloop
from .flash import decode_msg, flash_led
from zmq.eventloop.zmqstream import ZMQStream
from blink1.blink1 import blink1
from .config import DOWNSTREAM_URL

log = logging.getLogger(__name__)

ioloop.install()
context = zmq.Context()

def run():
    socket = context.socket(zmq.SUB)
    socket.connect(DOWNSTREAM_URL)
    log.info("Connecting to %s" % DOWNSTREAM_URL)

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

def main():
    logging.basicConfig()
    logging.getLogger("").setLevel(logging.INFO)
    run()

if __name__ == '__main__':
    main()
