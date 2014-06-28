import zmq
from itertools import count
from .config import UPSTREAM_PORT, DOWNSTREAM_PORT
import logging

log = logging.getLogger(__name__)


def run():
    """ main method """

    context = zmq.Context()

    upstream = context.socket(zmq.SUB)
    upstream_url = "tcp://*:%d" % UPSTREAM_PORT
    upstream.bind(upstream_url)
    log.info("Upstream bound to %s" % upstream_url)

    upstream.setsockopt_string(zmq.SUBSCRIBE, u"flash")

    downstream  = context.socket(zmq.PUB)
    downstream_url = "tcp://*:%d" % DOWNSTREAM_PORT
    downstream.bind(downstream_url)
    log.info("Downstream bound to %s" % downstream_url)

    try:
        for n in count():
            message = upstream.recv()
            downstream.send(message)
            log.info('%d: %s' % (n, message))
    finally:
        upstream.close()
        downstream.close()

    # try:
    #     log.info("Starting 0mq proxy.")
    #     zmq.device(zmq.QUEUE, frontend, backend)
    # finally:
    #     frontend.close()
    #     backend.close()
    #     context.term()

def main():
    logging.basicConfig()
    logging.getLogger("").setLevel(logging.INFO)
    run()

if __name__ == "__main__":
    main()