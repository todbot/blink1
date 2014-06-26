blink(1)  
========

"blink(1) is a super status light: it packs three dimensions of information 
(color, brightness and pattern) into a single tiny package that fits into 
any USB port on nearly every device. It makes it incredibly easy to connect 
any data source in the cloud or on your computer to a full-color RGB LED so 
you can know what's happening without checking any windows, going to any 
websites or typing any commands."

The official repository of blink(1) software and source code is:
https://github.com/todbot/blink1/


Ready-to-run Applications to play with
---------------------------------------

### Blink1Control ###

Blink1Control is a GUI application for hooking events to blink(1)
and is the primary user-level application for controlling blink(1).
It is an event-based system for triggering blink(1) color patterns.
Those events can be on your computer or on the Net.

Blin1kControl can receive events from:
- [IFTTT](http://ifttt.com/channels)'s many channels
- IMAP/POP/GMAIL email services
- URLs containing text or JSON
- Local files or executed scripts 
- Hardware monitors like CPU, battery, RAM

This verison of Blink1Control supercedes the Mac-specific and Windows-specific
versions in the "mac" and "win" directories.

- **Download Blink1Control** 
   - Mac OS X: [Blink1Control-mac.zip](https://github.com/todbot/blink1/releases)
   - Windows:  [Blink1Control-win.zip](https://github.com/todbot/blink1/releases)

- Previous versions of Blink1Control written in Cocoa or .NET (not recommended) -
[Blink1Control-mac-old.zip](http://thingm.com/blink1/downloads/old/Blink1Control-mac-old.zip) -
[Blink1Control-win-old.zip](http://thingm.com/blink1/downloads/old/Blink1Control-win-old.zip)

### Toy / Test Applications ###

And then these are more "toy" or "test" applications to just play around with your blink(1):
- BlinkATweet -- Watch real-time Twitter stream for keywords, flash blink(1) when found.  
Download:
[(Mac OSX)](http://thingm.com/blink1/downloads/BlinkATweet-mac.zip) /
[(Windows)](http://thingm.com/blink1/downloads/BlinkATweet-win.zip)

- Blink1ColorOrgan -- Sound-reactive program, music-to-color, instant disco!
Download:
[(Mac OSX)](http://thingm.com/blink1/downloads/Blink1ColorOrgan-mac.zip) /
[(Windows)](http://thingm.com/blink1/downloads/Blink1ColorOrgan-win.zip)

- Blink1ColorPicker -- Play with blink(1) select colors.
Download:
[(Mac OSX)](http://thingm.com/blink1/downloads/Blink1ColorPicker-mac.zip),
[(Windows)](http://thingm.com/blink1/downloads/Blink1ColorPicker-win.zip)

- Blink1Test0 -- a very simple random-color app. 
Download:
[(Mac OSX)](http://thingm.com/blink1/downloads/Blink1Test0-mac.zip) /
[(Windows)](http://thingm.com/blink1/downloads/Blink1Test0-win.zip)


### blink1-tool ###

For script-writers, programmers and hackers, there's the command-line tool:

- **blink1-tool** -- command-line program for use in your own scripts. 
  - Download for: 
[(Mac OSX)](http://thingm.com/blink1/downloads/blink1-tool-mac.zip) / 
[(Windows)](http://thingm.com/blink1/downloads/blink1-tool-win.zip) /
[(Linux64)](http://thingm.com/blink1/downloads/blink1-tool-linux_x86_64.zip) /
[(RaspberryPi)](http://thingm.com/blink1/downloads/blink1-tool-raspi.zip)


What is in this repo
--------------------
The following directories exist:

- docs             -- documentation about APIs and such
- commandline      -- blink1-lib C library and blink1-tool command-line tools
- java             -- Java / Processing library
- processing       -- Processing applications  (http://processing.org)
- python           -- Python APIs
- go               -- Go library
- libraries        -- libraries common to all systems (mostly just HTML app code)
- qt               -- Blink1Control lives here, along with some Qt sample apps
- mac              -- Mac OS X details and GUI control application
- mac/scripts      -- command-line scripts to do cool things
- windows          -- Windows details and GUI control application
- windows/Blink1Lib     -- .NET library wrapping blink1-lib
- windows/ManagedBlink1 -- 100% .NET library
- windows/scripts  -- Windows command-line scripts to do cool things
- linux            -- Linux details and scripts
- linux/contrib    -- contributed Linux scripts
- hardware         -- all hardware-related info (firmware & schematic & enclosure)


Some docs that might be useful
------------------------------

- [blink1-tool](https://github.com/todbot/blink1/blob/master/docs/blink1-tool.md)
- [app-url-api-examples](https://github.com/todbot/blink1/blob/master/docs/app-url-api-examples.md)
- [app-url-api](https://github.com/todbot/blink1/blob/master/docs/app-url-api.md)

Also see the [wiki pages](https://github.com/todbot/blink1/wiki/_pages) 


More Info about blink(1)
------------------------
http://blink1.thingm.com/


Other
-----

blink(1) is open source hardware

Copyright (c) ThingM, 2012-2014

'blink(1)' is a trademark of ThingM Corporation

License: CC BY-SA 3.0
http://creativecommons.org/licenses/by-sa/3.0/

