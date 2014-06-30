from socket import socket, AF_INET, SOCK_DGRAM
from .proxy import DISCOVERY_PORT, MAGIC_WORD
import logging
import json

log = logging.getLogger(__name__)

def discover():
    s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
    s.bind(('', DISCOVERY_PORT))

    log.info("Waiting for service anouncement broadcast on port %i" % DISCOVERY_PORT)

    while True:
        data, addr = s.recvfrom(1024) #wait for a packet
        message = data.decode()

        if message.startswith(MAGIC_WORD):
            result = json.loads(message[len(MAGIC_WORD)+1:])
            log.info("Got service anouncement %r" % result)
            return result
        else:
            log.info("Ignoring %r" % message)

if __name__ == '__main__':
    logging.basicConfig()
    logging.getLogger("").setLevel(logging.INFO)
    while True:
        print(discover())