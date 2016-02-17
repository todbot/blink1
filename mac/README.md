Blink1Control Mac OS X application for blink(1) 
===============================================

### NOTE: This Blink1Control is deprecated.  New development is in   blink1/qt/blink1control. 


-------

To build, open `blink1/mac/Blink1Control/Blink1Control.xcodeproj` in Xcode4.

All dependencies are included in the 'blink1' project.


Local webserver URL API:

See the document 'blink1/docs/app-url-api.md'


Releases 
========

0.990 -- 21 Dec 2012
- initial release to public

0.995 -- 23 Dec 2012
- fixed bug that allows running on 10.6

0.996 --
- added "Show dock icon" and "Menu icon matches blink(1)" menu items

0.997 -- 27 Jan 2013 
- fixed up IFTTT event sources
- cleaned up HTML UI a little




Open Source Software used in this application
=============================================

In general, the software packages are copied fairly wholesale, and only those
files relevant are added in the XCode build settings.  This means that even
if a package can create a Framework, it is not used, and the source files 
are used instead.

- blink1-lib - C-library to talking to blink(1), wraps HIDAPI
-- http://github.com/todbot/blink1

- CocoaHTTPServer - simple HTTP server
-- https://github.com/robbiehanson/CocoaHTTPServer

- RoutingHTTPServer - builds on CocoaHTTPServer for routing requests
-- https://github.com/mattstevens/RoutingHTTPServer

- SBJson - JSON Parser and Writer
-- http://stig.github.com/json-framework/

- SCEvents - FSEvents wrapper for Cocoa (with small changes)
-- http://stuconnolly.com/blog/fsevents-objectivec-wrapper/

- POLKit - For Task class, easily run shell scripts from Cocoa
-- https://code.google.com/p/polkit/

- PLWeakCompatibilityStubs.m -- for ARC compatibility on Mac OS X 10.6
-- https://github.com/plausiblelabs/PLWeakCompatibility


Compilation notes
=================
- ARC is turned off globally in Build setting, turned on per-file
 
