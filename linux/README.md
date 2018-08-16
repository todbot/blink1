
Controlling blink(1) devices on Linux
=====================================

In general you will need to install the udev rule file "51-blink1.rules",
so that a non-root user can talk to blink(1).  See that file for details.

## Command-line tool "blink1-tool"

To build the commamnd-line tool, you should be able to do:
  % cd ../commandline && make
and it will build a "blink1-tool" executable. 
There are dependencies on libusb-1.0.  See the Makefile for details.

Then to make the blink(1) do things, do something like:
  % ./blink1-tool --rgb 255,0,255

Type "./blink1-tool" by itself to get a help screen.


## Mini-HTTP server "blink1-tiny-server"

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


## Linux kernel support
In modern Linux kernels, you can enable `CONFIG_HID_THINGM` or `CONFIG_HID_LEDS`
to enable blink(1) support in the kernel.
https://github.com/torvalds/linux/blob/master/drivers/hid/hid-led.c

Some kernels, such as Raspbian for the Raspberry Pi has this enabled by default.

When you do, after plugging a blink(1) device in, you'll see messages like the below in syslog:

```
[352225.207493] usb 1-1.4: USB disconnect, device number 4
[352228.578796] usb 1-1.4: new full-speed USB device number 5 using dwc_otg
[352228.716576] usb 1-1.4: New USB device found, idVendor=27b8, idProduct=01ed
[352228.716590] usb 1-1.4: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[352228.716600] usb 1-1.4: Product: blink(1) mk2
[352228.716611] usb 1-1.4: Manufacturer: ThingM
[352228.716621] usb 1-1.4: SerialNumber: 200043D5
[352228.729441] hid-led 0003:27B8:01ED.0002: hidraw0: USB HID v1.01 Device [ThingM blink(1) mk2] on usb-3f980000.usb-1.4/input0
[352228.730616] hid-led 0003:27B8:01ED.0002: ThingM blink(1) initialized
```

And you'll see new directories in `/sys/class/leds/`:
```
pi@raspberrypi2:~ $ ls -1 /sys/class/leds/thingm*
led0
led1
thingm0:blue:led0
thingm0:blue:led1
thingm0:green:led0
thingm0:green:led1
thingm0:red:led0
thingm0:red:led1
```
By echoing values between 0-255 to the `brightness` entry`red`, `green`, or `blue` 


-eof-
