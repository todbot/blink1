
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


Mini-HTTP server "blink1-tiny-server"
---------------------------------------
There is a small web server based on Mongoose in the commandline directory
that is not built by default.  To build and run it:

```
cd blink1/commandline
make blink1-tiny-server
./blink1-tiny-server -p 8080
```

Control the blink1 via a few special URLs, which you can see with `./blink1-tiny-server -h`.

Example of using it: Turn the blink(1) blue over 5 seconds:

```
  % curl 'http://127.0.0.1:8080/blink1/fadeToRGB?rgb=%230000ff&time=5.00'
```
  
In this directory, there is an init script for starting `blink1-tiny-server`
at boot. See the comments in the file for how to install and use it.




-eof-
