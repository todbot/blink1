============
Blink1Control -- Mac OS X application for controlling blink(1) USB RGB LED
============

Local webserver URL API:

- /
  -- docroot for static HTML files hosted in "html"

- /blink1

  -- general status page for blink(1) devices connected to system
  -- returns:
   
- /blink1/list
  -- list blink(1)s
  -- response: list of blink(1) device ids
  -- response example: { "status" : "ok", 
                         "blink1_serialnums" : [ "FE0023","FD12CC" ] }
                                   
- /blink1/fadeToRGB
  -- params
      'rgb' = rgb hex color code
      'time' = time in float seconds
      'serialnum' = blink1 serial number to use, if omitted, use first blink(1)
  -- returns:
  -- example: ?rgb=#FF33dd&time=1.2
  
  
- /blink1/addPattern

- /blink1/listPatterns

- /blink1/playPattern
  - 

- /blink1/input/watchfile/?path=<file>
  -- can only watch one file at a time
  -- file is watched on 3 second interval
  -- when file changes (created or modified), it is parsed. 
  -- valid parse values: "color: #ff00ff" & "pattern: patternname"
  -- return: success or failure  (what is failure?  file non-exist?)

- /blink1/input/runscript/?cmd=<scriptpath>
  -- returns: output of script

- /blink1/input/audio/
  -- returns: audio output loudness 0-255

- /blink1/input/cpuload/
  -- returns: cpu load percent 0-255



=============================================
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


================= 
Compilation notes
=================
- ARC is turned off globally in Build setting, turned on per-file
