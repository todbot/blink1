FROM ubuntu
MAINTAINER Rob Powell <rob.p.tec@gmail.com>

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update

RUN apt-get install -y \
        build-essential \
        pkg-config \
        libusb-1.0-0-dev


RUN mkdir /home/blinkdev

ENV HOME /home/blinkdev

COPY . ${HOME}/commandline

WORKDIR ${HOME}/commandline

RUN make

ENV PATH ${HOME}/commandline:${PATH}

CMD ["blink1-tool", "-t 1000", "--random=100"]