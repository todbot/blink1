
Controlling blink(1) devices on Linux
=====================================

In general you will need to install the udev rule file "51-blink1.rules",
so that a non-root user can talk to blink(1).  See that file for details.

To build the commamnd-line tool, you should be able to do:
  % cd ../commandline && make
and it will build a "blink1-tool" executable. 
There are dependencies on libusb-1.0.  See the Makefile for details.

Then to make the blink(1) do things, do something like:
  % ./blink1-tool --rgb 255,0,255

Type "./blink1-tool" by itself to get a help screen.


-eof-
