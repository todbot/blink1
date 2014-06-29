import logging
import zmq
import click
from zmq.eventloop import ioloop
from .controller import Receiver, DEFAULT_CHANNEL
from zmq.eventloop.zmqstream import ZMQStream
from blink1.blink1 import blink1
from .discover import discover

log = logging.getLogger(__name__)

ioloop.install()
context = zmq.Context()

@click.command()
@click.option('--white_point', default='halogen', help='Blink(1) white-point adjustment')
def run(white_point):
    config = discover()
    downstream_url = config['downstream']

    socket = context.socket(zmq.SUB)
    socket.connect(config['downstream'])
    log.info("Connecting to %s" % downstream_url)

    socket.setsockopt_string(zmq.SUBSCRIBE, DEFAULT_CHANNEL)
    stream = ZMQStream(socket)

    loop = ioloop.IOLoop.instance()

    with blink1(white_point=white_point) as b1:
        reciever = Receiver(b1, loop)
        stream.on_recv(reciever.recieve)

        loop.add_callback(reciever.throbber)

        loop.start()

def main():
    logging.basicConfig()
    logging.getLogger("").setLevel(logging.INFO)
    run()

if __name__ == '__main__':
    main()
