
Tips and Tricks for blink1-tool 
===============================

Here are some examples of things you can do with blink(1)
and the `blink1-tool` command-line program.

You can get the `blink1-tool` program for your operating system from:
  https://github.com/todbot/blink1/releases

Or you can compile it yourself by cloning the blink1 repo at:
  https://github.com/todbot/blink1


This document includes tips separated into two sections:


Using blink1-tool:
* [Getting blink1-tool working](#Astart)
* [First commands](#Afirst)
* [Setting custom colors with '--rgb'](#Acolors)
* [Blinking colors with '--blink'](#Ablink)
* [Changing blink speed with '--delay' and '--millis'](#Adelay)
* [Finding and listing blink(1) devices](#Alist)
* [Controlling multiple blink(1) devices](#Amulti)

Writing scripts with blink1-tool:
* [Mac/Linux: Turn on one LED, then the other](#A10)
* [Windows: Turn on one LED, then the other](#A11)
* [Make all blink(1)s blink orange 5 times](#A12)
* [Turn all blink(1)s the same color](#A13)
 
Also check out the 'blink1/commandline/scripts' directory for more examples:
  https://github.com/todbot/blink1/tree/master/commandline/scripts 

<a name="Astart"></a>
### Getting `blink1-tool` working

The first thing to do is to make sure you can run `blink1-tool`
and find blink(1) devices.  Once you download and unzip the blink1-tool zip for
your OS (or build it yourself).  Try running it with no options.  It will give you
a help screen.

On Mac and Linux: Open up Terminal and 'cd' to the directory where `blink1-tool` is.
For instance, if you unzipped into your Downloads folder, type:

    cd $HOME/Downloads
    ./blink1-tool

On Windows: Run PowerShell or a DOS command window and get into the directory
where `blink1-tool` lives.  For instance, if you unzipped into your Downloads folder, type:

    cd %HOMEPATH%/Downloads
    blink1-tool.exe

On Linux, if you are running as non-root and haven't installed the blink1-udev-rules, you will need to use `sudo` with:

    sudo ./blink1-tool

In all the examples below, when `blink-tool` is specified, it means either `./blink1-tool` or `blink1-tool.exe`, or `sudo ./blink1-tool` depending on your OS.


<a name="Afirst"></a>
## First commands with blink1-tool

Now that you can run `blink1-tool`, try out some of the built in color commands.

All commands start with `--`. For example:

    blink1-tool --on    # turn blink(1) on full-white
    blink1-tool --off   # turn blink(1) off
    blink1-tool --red
    blink1-tool --green
    blink1-tool --blue
    blink1-tool --cyan
    blink1-tool --magenta
 

<a name="Acolors"></a>
## Setting custom colors with '--rgb'

The main way to set colors is with the `--rgb` command.
The `--rgb` option can take an red,green,blue triplet or a hex color code.

For instance, all these commands do the exact same thing to turn the blink(1) orange:

    blink1-tool --rgb 255,204,0        # turn orange
    blink1-tool --rgb 0xff,0xcc,0x00   # turn orange also
    blink1-tool --rgb ffcc00           # again, turn orange
    blink1-tool --rgb #FFCC00          # hey I know, turn orange
    

<a name="Ablink"></a>
## Blinking colors with '--blink'

One of the most used commands is `--blink`. It blinks a specific color a number of times.
For instance, if you want to blink red 5 times for a red alert, do either:

    blink1-tool --rgb #FF0000 --blink 5 
    blink1-tool --red --blink 5


<a name="Adelay"></a>
## Changing blink speed using '--delay' and '--millis'

If you want to make it blink faster, use the `--delay` option.
Delay is used to specify the time between events in blink1-tool.
The default delay value is 500 (500 milliseconds).
Too make the blinking faster, use a smaller value, like 100.

    blink1-tool --delay 100 --rgb #FF0000 --blink 5 

But if you do that you'll see the blink(1) doesn't go completely dark when blinking.
This is because there's another time parameter you can set: the fade time.
The fade time is what  blink(1) uses to smoothly fade from the current color
to the new color.  Set it with `--millis`. It defaults to 300.
Set `--delay` shorter for more abrupt changes.

So here's a better red-alert:

    blink1-tool --delay 100 --millis 50 --rgb #FF0000 --blink 5

Usually you want `--millis` fade time to be 1/2 `--delay`.
These two options have short form: `-d` for `--delay` and `-m` for `--millis`.
The above could instead be written:

    blink1-tool -d 100 -m 50 --rgb #FF0000 --blink 5


<A name="Alist"></a>
## Finding and listing blink(1) devices

You can have multiple blink(1) devices and address each one individually.
This is useful if you want to dedicate different blink(1) devices to mean different
things, like one-per-person in your continuous integration monitor or one-per-server
for network monitoring.

To find and list the all the blink(1) devices currently connected, use `--list`.
Below is shows what you will see with four blink(1)s connected.

    % blink1-tool --list
    blink(1) list:
    id:0 - serialnum:2143AB23 (mk2)
    id:1 - serialnum:2143DC05 (mk2)
    id:2 - serialnum:214369F0 (mk2)
    id:3 - serialnum:2143BADC (mk2)

<a name="Amulti"></a>
## Controlling multiple blink(1) devices

To control a specific blink(1), identify it with either its "id" number
(0,1,2,3 in the above example) or its serial number.  The `--id` (short form: `-d`)
option is used to specify.  For example, to control the blink(1) with id=2 from
the previous example, do:

    blink1-tool --id 2 --rgb #00FF00 --blink 3

The `--id` option takes a comma-separated list of ids/serials, so if you want to
control just blink(1)s with id=1 and id=3 from above, do:

    blink1-tool --id 1,3 --rgb #00FF00 -- blink 3

A special case is `--id all` which will control all currently connected blink(1) devices.
This is great when you want to quickly set all blink(1)s to a color:

    blink1-tool --id all --red


<br>
<br>
<br>
<br>
<hr>
<br>
<br>
<br>
<br>
<br>




<a name="A10"></a>
## Mac/Linux: Turn on one LED, then the other

This examples uses the "-l" option to specify which LED to light.
The default is "-l 0", which means all LEDs on a single blink(1) mk2.
Note the "-l" option works with most, but not all, commands in blink1-tool.

    while [ 1 ] ; do
        blink1-tool -l 1 --red  && blink1-tool -l 2 --blue
        sleep 1
        blink1-tool -l 1 --blue && blink1-tool -l 2 --red
        sleep 1
    done


<a name="A11"></a>
## Windows: Turn on one LED, then the other

This examples uses the "-l" option to specify which LED to light.
The default is "-l 0", which means all LEDs on a single blink(1) mk2.
Note the "-l" option works with most, but not all, commands in blink1-tool.

This example uses a Windows DOS batch file:

    @echo off
    REM Select your path to blink1-tool.exe 

    SET PATH=%PATH%;c:\portable

    set /a "x = 0"
    :while
    if %x% leq 7 (
        echo " %x%
        set /a "x = x + 1"
	    blink1-tool.exe -l 1 --red
	    blink1-tool.exe -l 2 --blue
	    timeout /T 1 > NUL

	    blink1-tool.exe -l 1 --blue
	    blink1-tool.exe -l 2 --red
	    timeout /T 1 > NUL

	    goto :while
    )
    endlocal


<a name="A12"></a>
## Make all blink(1)s blink orange 5 times

This example uses the existing "-d" option to specify which blink(1) to address.
The '-d' option takes the "id" value given by the "--list" commmand,
or "-all" to mean all blink(1)s.

    % blink1-tool --list
    blink(1) list:
    id:0 - serialnum:2143AB23 (mk2)
    id:1 - serialnum:2143DC05 (mk2)
    id:2 - serialnum:214369F0 (mk2)
    id:3 - serialnum:2143BADC (mk2)

    % blink1-tool -d all --rgb #FF9900 --blink 5


## Turn all blink(1)s to the same color

This example uses the existing "-d" option to specify which blink(1) to address.
The '-d' option takes the "id" value given by the "--list" commmand,
or "-all" to mean all blink(1)s.

    % blink1-tool --list
    blink(1) list:
    id:0 - serialnum:2143AB23 (mk2)
    id:1 - serialnum:2143DC05 (mk2)
    id:2 - serialnum:214369F0 (mk2)
    id:3 - serialnum:2143BADC (mk2)

    % blink1-tool -d all --rgb 255,0,255
    set dev:0 to rgb:0xff,0x00,0xff over 300 msec
    set dev:1 to rgb:0xff,0x00,0xff over 300 msec
    set dev:2 to rgb:0xff,0x00,0xff over 300 msec
    set dev:3 to rgb:0xff,0x00,0xff over 300 msec


Play the green part of the startup sequence 3 times
---------------------------------------------------

    % blink1-tool --play 1,3,5,3
    playing color pattern from pos 3-5 (3 times)


Play the blue part of the startup sequence forever
--------------------------------------------------

    % blink1-tool --play 1,6,8,0
    playing color pattern from pos 6-8 (0 times)


Stop playing
------------
You can use either "--stop" or "--play 0" to stop the playing light pattern.
The latter may be more useful if you're using blink1-tool in a programmatic context.

    % blink1-tool --stop
    % blink1-tool --play 0  # equivalent


Dump current (in RAM) light pattern
-----------------------------------
To get the light pattern stored in a blink(1), use the "--getpattline <n>" command
to get a pattern line.  The number of pattern lines available is 12 (for mk1 devices)
and 32 (for mk2 devices).

    % for p in {0..15}
      do
        blink1-tool --getpattline $p
      done
    reading rgb at pos 0: r,g,b = 0xff,0x00,0x00 millis:500
    reading rgb at pos 1: r,g,b = 0xff,0x00,0x00 millis:500
    reading rgb at pos 2: r,g,b = 0x00,0x00,0x00 millis:500
    reading rgb at pos 3: r,g,b = 0x00,0xff,0x00 millis:500
    reading rgb at pos 4: r,g,b = 0x00,0xff,0x00 millis:500
    reading rgb at pos 5: r,g,b = 0x00,0x00,0x00 millis:500
    reading rgb at pos 6: r,g,b = 0x00,0x00,0xff millis:500
    reading rgb at pos 7: r,g,b = 0x00,0x00,0xff millis:500
    reading rgb at pos 8: r,g,b = 0x00,0x00,0x00 millis:500
    reading rgb at pos 9: r,g,b = 0xff,0xff,0xff millis:1000
    reading rgb at pos 10: r,g,b = 0x00,0x00,0x00 millis:1000
    reading rgb at pos 11: r,g,b = 0xff,0xff,0xff millis:500
    reading rgb at pos 12: r,g,b = 0x00,0x00,0x00 millis:500
    reading rgb at pos 13: r,g,b = 0xff,0xff,0xff millis:500
    reading rgb at pos 14: r,g,b = 0x00,0x00,0x00 millis:1000
    reading rgb at pos 15: r,g,b = 0x00,0x00,0x00 millis:1000


Infinite Rainbow using Bash Shell
---------------------------------
Use the new "--hsb" command to specify colors by HSB (hue, saturation, brightness)
instead of the standard "--rgb" command.  Note that the HSB-to-RGB conversion
is done within blink1-tool, not in the blink(1) device.

    while [ 1 ]
      do
        for h in {0..255..16}
        do
          blink1-tool --hsb $h,255,255
          sleep 0.3
        done
      done


Infinite Rainbow Using Batch file in Windows 
------------------------------------------------------
Use the "--hsb" command to specify colors by HSB (hue, saturation, brightness)
instead of the standard "--rgb" command.  Note that the HSB-to-RGB conversion
is done within blink1-tool, not in the blink(1) device.

Here's an example in a Windows DOS batch file:

    @echo off
    REM Select your path to blink1-tool.exe 
    REM Use millisleep.exe  from http://www.elifulkerson.com/projects/millisleep.php to slow down to 0.3 seconds
        SET PATH=%PATH%;c:\portable
        set /a "x = 0"
        :while
            set /a "x = x + 16"
	        blink1-tool --hsb %x%,255,255
	        millisleep 300 > NUL
	    goto :while
    endlocal


Write a rainbow color pattern
-----------------------------
This example only writes the color pattern to RAM, these changes are lost
if you unplug.  See below for how to save to flash.

    % for h in {0..255..16}
      do
        p=`expr $h / 16`
        blink1-tool --hsb $h,255,255 --setpattline $p
      done
      
    saving rgb: 0xff,0x00,0x00 @ 0, ms:300
    saving rgb: 0xff,0x60,0x00 @ 1, ms:300
    saving rgb: 0xff,0xc0,0x00 @ 2, ms:300
    saving rgb: 0xe1,0xff,0x00 @ 3, ms:300
    saving rgb: 0x81,0xff,0x00 @ 4, ms:300
    saving rgb: 0x21,0xff,0x00 @ 5, ms:300
    saving rgb: 0x00,0xff,0x3c @ 6, ms:300
    saving rgb: 0x00,0xff,0x9c @ 7, ms:300
    saving rgb: 0x00,0xff,0xfc @ 8, ms:300
    saving rgb: 0x00,0xa5,0xff @ 9, ms:300
    saving rgb: 0x00,0x45,0xff @ 10, ms:300
    saving rgb: 0x18,0x00,0xff @ 11, ms:300
    saving rgb: 0x78,0x00,0xff @ 12, ms:300
    saving rgb: 0xd8,0x00,0xff @ 13, ms:300
    saving rgb: 0xff,0x00,0xc9 @ 14, ms:300
    saving rgb: 0xff,0x00,0x69 @ 15, ms:300

    % blink1-tool --play 1

Save color pattern to non-volatile flash 
----------------------------------------
This copies RAM pattern to flash, making it available on next power up.
Note that only the first 16 of 32 pattern lines are saved to flash.

    % blink1-tool --savepattern
    

Is blink(1) playing a color pattern?
------------------------------------

    % blink1-tool --playstate
    playstate: playing:1 start-end:0-16 count:0 pos:4


Start pattern playing, wait for it to stop
------------------------------------------

    # play red & green part of default pattern 3 times
    % blink1-tool --play 1,0,5,3
    # wait for "playing" to go to zero before continuing
    % while [ `blink1-tool --playstate | cut -c 20` != 0 ]
      do
        echo "still playing"
      done
    # this executes when blink1-tool finishes playing
    echo "done!"


Use the 'serverdown' to turn on if your server dies
---------------------------------------------------
This will run infinitely, displaying green showing that things are okay,
and always keeping the blink(1) from not playing its color pattern.  
But if the computer hangs, then after 2 seconds (2000 msecs)
the color pattern will play.

    while [ 1 ] 
      do
        blink1-tool --green   # all good
        blink1-tool -t 2000 --servertickle 1,1
        sleep 1
      done

