
blink(1) HID commands
=====================

Communication with blink(1) is done via USB HID Feature Reports.


## Command Summary ##

    - Fade to RGB color       format: {1, 'c', r,g,b,      th,tl, 0 }
    - Set RGB color now       format: {1, 'n', r,g,b,        0,0, 0 }
    - Serverdown tickle/off   format: {1, 'D', on,th,tl,     0,0, 0 }
    - Play/Pause              format: {1, 'p', on,p,0,       0,0, 0 }
    - Set color pattern line  format: {1, 'P', r,g,b,      th,tl, p }
    - read color pattern line format: {1, 'R', 0,0,0,        0,0, p }
    - Read EEPROM location    format: {1, 'e', ad,0,0,       0,0, 0 }
    - Write EEPROM location   format: {1, 'E', ad,v,0,       0,0, 0 }
    - Get version             format: {1, 'v', 0,0,0,        0,0, 0 } 
    - Test                    format: {1, '!', 0,0,0,        0,0, 0 }

where:

    r,g,b = 24-bit RGB color
       th = (fadetimeMillis/10) >> 8
       tl = (fadetimeMillis/10) & 0xff
       on = 1 or 0, indicating on/off, play/pause, etc.
        p = position in a list, starting at 0
       ad = address starting at 1
        v = arbitrary value 


Protocol
--------

blink(1) takes 8 bytes of input

    - byte 0 = report_id (0x01)
    - byte 1 = command action ('c' = fade to rgb, 'v' get firmware version, etc.)
    - byte 2 = cmd arg 0 (e.g. red)
    - byte 3 = cmd arg 1 (e.g. green)
    - byte 4 = cmd arg 2 (e.g. blue)
    - byte 5 = cmd arg 3 (e.g. th)
    - byte 6 = cmd arg 4 (e.g. tl)
    - byte 7 = cmd arg 5 (e.g. step)

Some things to note:

- The command action is the ascii value of the letter given (e.g. 'c' = 0x63)
- Any command with unused cmd args should set them to zero.


The most common command is "fade to RGB", which has the form:

    - byte 0 = 0x01  (report_id )
    - byte 1 = 'c'   (command "fade to rgb")
    - byte 2 = red value
    - byte 3 = green value
    - byte 4 = blue value
    - byte 5 = th    (fadeMillis/10 high byte)
    - byte 6 = tl    (fadeMillis/10 high byte)
    - byte 7 = 0     (unused)

(th: time/cs high (T >>8)  where time 'T' is a number of 10msec ticks
tl : time/cs low (T & 0xff)


 
Examples
--------

## fade to white (255,255,255) in 5 seconds

{ 1, 'c', 0xff,0ff,0xff, 0x01,0xff, 0 }



