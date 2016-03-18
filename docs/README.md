Device API Documentation for blink(1)
=====================================

There are multiple ways to communicate with a blink(1) device.

- Raw HID SetFeature & GetFeature reports
- C API 
- Java / Processing API
- Command-line API
- Local Web API
- IFTTT API

Raw HID SetFeature & GetFeature reports
---------------------------------------
If you like doing down-to-the-metal SetFeature and GetFeature USB HID reports,
blink(1) speaks a simple 8-byte report for both.  

See the C API for details.


C API
-----
For the C API, check out the [blink1-lib.h](https://github.com/todbot/blink1/blob/master/commandline/blink1-lib.h) 
and [blink1-lib.c](https://github.com/todbot/blink1/blob/master/commandline/blink1-lib.c) files 
in the "commandline" directory.


Java / Processing API
---------------------
Check out the set of Java / Processing examples in the "processing" directory.

For ready-to-run Processing examples using the Java library, download and install:
[blink1-java-processing-lib.zip](http://thingm.com/blink1/downloads/blink1-java-processing-lib.zip)


Command-line API
----------------
The command-line program "blink1-tool" lets you have full control over
one or more blink(1) devices.  You can use this program inside of 
shell scripts or batch files, or in a pinch, call them from within PHP
web pages.

It is documented in [blink1-tool](https://github.com/todbot/blink1/blob/master/docs/blink1-tool.md).


Local Web API
-------------
The local Web API is a REST API with JSON responses that comes with
the Mac OS X and Windows control applications.

It is documented in [app-url-api](https://github.com/todbot/blink1/blob/master/docs/app-url-api.md).
And has a set of step-by-step examples in [app-url-api-examples](https://github.com/todbot/blink1/blob/master/docs/app-url-api-examples.md).


IFTTT API
---------
You can use IFTTT.com to hook up all sorts of Net-based data sources to
your blink(1).  Details on that coming shortly.


