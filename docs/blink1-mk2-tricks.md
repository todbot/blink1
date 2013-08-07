
blink(1) mk2 blink1-tool tricks
===============================

Examples of things you can do with blink(1) mk2
and the blink1-tool command-line program on a bash shell.


Turn on one LED, then the other
-------------------------------
    % while [ 1 ] ; do
        blink1-tool -l 1 --red  && blink1-tool -l 2 --blue
        sleep 1
        blink1-tool -l 1 --blue && blink1-tool -l 2 --red
        sleep 1
      done


Turn all plugged-in blink(1)s to the same color
-----------------------------------------------

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
    % blink1-tool --stop
    % blink1-tool --play 0  # equivalent


Dump current (in RAM) light pattern
-----------------------------------
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

    % while [ 1 ]
      do
        for h in {0..255..16}
        do
          blink1-tool --hsb $h,255,255
          sleep 0.3
        done
      done


Write a rainbow color pattern
-----------------------------
This only writes it to RAM, these changes are lost
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
This copies RAM pattern to flash.

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


