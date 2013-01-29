GoBlink, A Go interface to the Blink(1) USB device
=======

This project demonstrates intefacing to a USB device in Go, the [ThingM blink(1)](http://thingm.com/products/blink-1.html). The original interfacing examples are written in C, so this project uses [CGO](http://golang.org/cmd/cgo/) to integrate C code into Go.

## Set up ##

The code was developed and tested on Mac OSX Mountain Lion, but uses nothing OS-specific, so should also work on Linux. 

To run, you'll first need to install *libusb*. The easiest way to so is via [MacPorts](https://www.macports.org).

1. Install [Xcode](https://developer.apple.com/xcode/) and the [Command Line Tools for Xcode](https://developer.apple.com/downloads/index.action)
2. Install [MacPorts](https://www.macports.org/install.php)
3. Now use MacPorts to install *libusb*:

	$ sudo port install libusb-compat
	
	If your MacPorts installation does not install the *libusb* files into */opt/local/{include | lib}*, then adjust the *#cgo* compiler directives in *hid.go*. 

4. *cd* to the directory with this project's files, then to build and run

	$ go build goBlink.go && ./goBlink


## TODOs ##

Several functions are shown, including blink, set, and random. Others can be added such as read and write.


## Sources ##
The code is based on the C code in the [blink1 repository](https://github.com/todbot/blink1), particularly the [blink1-mini-tool](https://github.com/todbot/blink1/tree/master/commandline/blink1-mini-tool). Christian Starkjohann's *hiddata.c* is used almost as is. The modifications are to use *enum*s instead of *#define*s to make the values available in Go. Also, the single-inclusion *#define*s needed for C are not needed for Go and excised.

See also the other examples in the [command line directory](https://github.com/todbot/blink1/tree/master/commandline). 


