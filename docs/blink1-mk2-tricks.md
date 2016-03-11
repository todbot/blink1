
blink(1) mk2 blink1-tool tricks 
===============================

Examples of things you can do with blink(1) mk2
and the blink1-tool command-line program.


Turn on one LED, then the other
-------------------------------
This examples uses the "-l" option to specify which LED to light.
The default is "-l 0", which means all LEDs on a single blink(1) mk2.
Note the "-l" option works with most, but not all, commands in blink1-tool.

    while [ 1 ] ; do
        blink1-tool -l 1 --red  && blink1-tool -l 2 --blue
        sleep 1
        blink1-tool -l 1 --blue && blink1-tool -l 2 --red
        sleep 1
    done


Turn on one LED, then the other with batch file in Windows
------------------------------------------------------
This examples uses the new "-l" option to specify which LED to light.
The default is "-l 0", which means all LEDs on a single blink(1) mk2.
Note the "-l" option works with most, but not all, commands in blink1-tool.

This example uses a Windows DOS batch file:

    @SETLOCAL
    @ECHO OFF
    
    ::
    :: Select your path to blink1-tool.exe 
    ::
    SET PATH=%PATH%;c:\portable
    
    FOR /L %%i IN (1,1,7) DO (
        echo %%i
        blink1-tool.exe -l 1 --red  
        blink1-tool.exe -l 2 --blue
        timeout /T 1 > NUL
    
        blink1-tool.exe -l 1 --blue
        blink1-tool.exe -l 2 --red
        timeout /T 1 > NUL
    )


Turn all plugged-in blink(1)s to the same color
-----------------------------------------------
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


Infinite Rainbow using Bash
---------------------------
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
This will run infinitely, always keeping the blink(1) from not playing
its color pattern.  But if the computer hangs, then after 2 seconds (2000 msecs)
the color pattern will play.

    while [ 1 ] 
      do
        blink1-tool -t 2000 --servertickle
        sleep 1
      done

