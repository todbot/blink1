
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

C API
-----
For the C API, check out the "blink1-lib.h" and "blink1-lib.c" files.  

For an example of them in use, see the command-line tool "blink1-tool.c".


Java / Processing API
---------------------
Check out the 

Command-line API
----------------
The command-line program "blink1-tool" lets you have full control over
one or more blink(1) devices.  You can use this program inside of 
shell scripts or batch files, or in a pinch, call them from within PHP
web pages.

It is documented in [blink1-tool](blink1-tool.md).


Local Web API
-------------
The local Web API is a REST API with JSON responses that comes with
the Mac OS X and Windows control applications.

It is documented in "app-url-api".
And has a set of step-by-step examples in "app-url-api-examples".


IFTTT API
---------
You can use IFTTT.com to hook up all sorts of Net-based data sources to
your blink(1).  Details on that coming shortly.


