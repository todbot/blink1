
Controlling blink(1) devices on Linux
=====================================

In general you will need to install the udev rule file "51-blink1.rules",
so that a non-root user can talk to blink(1).  See that file for details.

Command-line tool "blink1-tool"
-------------------------------
To build the commamnd-line tool, you should be able to do:
  % cd ../commandline && make
and it will build a "blink1-tool" executable. 
There are dependencies on libusb-1.0.  See the Makefile for details.

Then to make the blink(1) do things, do something like:
  % ./blink1-tool --rgb 255,0,255

Type "./blink1-tool" by itself to get a help screen.


Mini-HTTP server "blink1-server-simple"
---------------------------------------
There is a small web server based on Mongoose in the commandline directory
that is not built by default.  Do a "make blink1-server-simple" to build it.
If you run it, it will sit on port 8080 and control the blink1 via a few
special URLs.  

For example, to turn the blink(1) blue over 5 seconds:
  % curl 'http://127.0.0.1:8080/blink1/fadeToRGB?rgb=%230000ff&time=5.00'




-eof-
