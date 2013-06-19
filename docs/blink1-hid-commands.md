
blink(1) HID commands
=====================

Communication with blink(1) is done via USB HID Feature Reports.
This document shows the format of the data packet sent to blink(1).


## Command Summary ##

    - Fade to RGB color       format: {0x01, 'c', r,g,b,    th,tl, 0 }
    - Set RGB color now       format: {0x01, 'n', r,g,b,      0,0, 0 }
    - Read current RGB color  format: {0x01, 'C', n,0,0,      0,0, 0 } (*)
    - Serverdown tickle/off   format: {0x01, 'D', on,th,tl,  st,0, 0 } (*)
    - Play/Pause              format: {0x01, 'p', on,p,0,     0,0, 0 }
    - Set color pattern line  format: {0x01, 'P', r,g,b,    th,tl, p }
    - Save color patterns     format: {0x01, 'W', 0,0,0,      0,0, 0 } (*)
    - read color pattern line format: {0x01, 'R', 0,0,0,      0,0, p }
    - Read EEPROM location    format: {0x01, 'e', ad,0,0,     0,0, 0 }
    - Write EEPROM location   format: {0x01, 'E', ad,v,0,     0,0, 0 }
    - Get version             format: {0x01, 'v', 0,0,0,      0,0, 0 } 
    - Test command            format: {0x01, '!', 0,0,0,      0,0, 0 }

where:

    r,g,b = 24-bit RGB color
       th = (fadetimeMillis/10) >> 8
       tl = (fadetimeMillis/10) & 0xff
       on = 1 or 0, indicating on/off, play/pause, etc.
       st = 1 or 0, indicating on/off, maintain state or clear state (mk2 only)
        p = position in a list, starting at 0
       ad = address starting at 1
        v = arbitrary value 

      (*) some or all of this command is only available in mk2 firmware


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
    - byte 6 = tl    (fadeMillis/10 low byte)
    - byte 7 = 0     (unused)

(th: time/cs high (T >>8)  where time 'T' is a number of 10msec ticks
tl : time/cs low (T & 0xff)


Commands
--------

### Fade To RGB - `format: {0x01, 'c', r,g,b,    th,tl, 0 }`
This command does not produce a return value

    - byte 0 = 0x01  (report_id )
    - byte 1 = 'c'   (command "fade to rgb")
    - byte 2 = red value
    - byte 3 = green value
    - byte 4 = blue value
    - byte 5 = th    (fadeMillis/10 high byte)
    - byte 6 = tl    (fadeMillis/10 low byte)
    - byte 7 = 0     (unused)

### Servertickle - `format: {0x01, 'D', on,th,tl,  st,0, 0 }`

    - byte 0 = 0x01  (report_id )
    - byte 1 = 'D'   (command "servertickle")
    - byte 2 = {1,0} (1 = enable servertickle, 0 = disable servertickle)
    - byte 3 = th    (timeout/10 high byte)
    - byte 4 = tl    (timeout/10 low byte)
    - byte 5 = {1,0} (1 = maintain state, 0 = reset to off (mk2 firmware only))
    - byte 6 = 0     (unused)
    - byte 7 = 0     (unused)


Examples
--------

* fade to white (255,255,255) in 5 seconds

  `{ 0x01, 'c', 0xff,0ff,0xff, 0x01,0xff, 0 }`


* fade to white (255,255,255) in 5 seconds

  `{ 0x01, 'c', 0xff,0ff,0xff, 0x01,0xff, 0 }`


