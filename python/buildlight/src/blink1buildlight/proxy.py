import zmq
import socket
import datetime
import json
from itertools import count
from zmq.eventloop import ioloop
from zmq.eventloop.zmqstream import ZMQStream
import logging

log = logging.getLogger(__name__)

DISCOVERY_PORT = 6969
MAGIC_WORD = "Build Light Controller"
PING_FREQUENCY = 10

ioloop.install()
context = zmq.Context()

def run():
    """ main method """
    hostname = socket.gethostname()

    context = zmq.Context()


    # Configure the upstream socket
    upstream = context.socket(zmq.SUB)
    upstream_port = upstream.bind_to_random_port('tcp://*')
    upstream_url = "tcp://%s:%d" % (hostname, upstream_port)
    log.info("Upstream bound to %s" % upstream_url)

    upstream.setsockopt_string(zmq.SUBSCRIBE, u"buildlight")

    # Configure the downstream socket
    downstream  = context.socket(zmq.PUB)
    downstream_port = downstream.bind_to_random_port('tcp://*')
    downstream_url = "tcp://%s:%d" % (hostname, downstream_port)
    log.info("Downstream bound to %s" % downstream_url)

    ping_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    ping_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    ping_socket.bind(('', 0))

    loop = ioloop.IOLoop.instance()

    def ping(serial_number=0):
        ps = ping_socket
        data = json.dumps({
            'upstream':upstream_url,
            'downstream':downstream_url,
            'serial_number':serial_number
        })
        message = "%s\n%s" % (MAGIC_WORD, data)
        ps.sendto(message.encode(), ('<broadcast>', 6969))

        # Let's do this again some time?
        callback = lambda : ping(serial_number=serial_number + 1)
        loop.add_timeout(datetime.timedelta(seconds=PING_FREQUENCY), callback)
        log.info("Ping: %s" % message)


    loop.add_callback(ping)

    def recieve(msg):
        for m in msg:
            downstream.send(m)
            log.info('Proxied: %s' % (m))

    zstr = ZMQStream(upstream)
    zstr.on_recv(recieve)

    try:
        loop.start()
    finally:
        upstream.close()
        downstream.close()


def main():
    logging.basicConfig()
    logging.getLogger("").setLevel(logging.INFO)
    run()

if __name__ == "__main__":
    main()