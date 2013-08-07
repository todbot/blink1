
blink(1) HID commands
=====================

Communication with blink(1) is done via USB HID Feature Reports.
This document shows the format of the data packet sent to blink(1).


## Command Summary ##

    - Fade to RGB color       format: {0x01, 'c', r,g,b,     th,tl, n } (*)
    - Set RGB color now       format: {0x01, 'n', r,g,b,       0,0, n } (*)
    - Read current RGB color  format: {0x01, 'r', n,0,0,       0,0, n } (2)
    - Serverdown tickle/off   format: {0x01, 'D', on,th,tl,  st,sp,ep } (*)
    - Play/Pause              format: {0x01, 'p', on,sp,0,     0,0, 0 }
    - PlayLoop                format: {0x01, 'p', on,sp,ep,c,    0, 0 } (2)
    - Playstate readback      format: {0x01, 'S', 0,0,0,       0,0, 0 } (2)
    - Set color pattern line  format: {0x01, 'P', r,g,b,     th,tl, p }
    - Save color patterns     format: {0x01, 'W', 0,0,0,       0,0, 0 } (2)
    - read color pattern line format: {0x01, 'R', 0,0,0,       0,0, p }
    - Read EEPROM location    format: {0x01, 'e', ad,0,0,      0,0, 0 } (1)
    - Write EEPROM location   format: {0x01, 'E', ad,v,0,      0,0, 0 } (1)
    - Get version             format: {0x01, 'v', 0,0,0,       0,0, 0 }
    - Test command            format: {0x01, '!', 0,0,0,       0,0, 0 }

where:

    r,g,b = 24-bit RGB color
        n = which LED to address: 0=all, 1=led#1, 2=led#2, etc. (mk2 only)
       th = (fadetimeMillis/10) >> 8
       tl = (fadetimeMillis/10) & 0xff
       on = 1 or 0, indicating on/off, play/pause, etc.
       st = 1 or 0, indicating on/off, maintain state or clear state (mk2 only)
        p = position in list, starts at 0, goes to patt_max-1 (patt_max=12(mk1),16(mk2))
       sp = start loop position (0 - patt_max-1)
       ep = end loop position (1 - patt_max)
       ad = address starting at 1
        v = arbitrary value 

      (*) some arguments for command is only available in mk2 devices
      (2) mk2 devices only
      (1) mk1 devices only


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
    - byte 7 = cmd arg 5 (e.g. ledn)

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
    - byte 7 = 0     (unused on mk1, 0=all on mk2) 

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
    - byte 7 = 0     (unused on mk1)

### Servertickle - `format: {0x01, 'D', on,th,tl,  st,0, 0 }`
This command does not produce a return value

    - byte 0 = 0x01  (report_id )
    - byte 1 = 'D'   (command "servertickle")
    - byte 2 = {1,0} (1 = enable servertickle, 0 = disable servertickle)
    - byte 3 = th    (timeout/10 high byte)
    - byte 4 = tl    (timeout/10 low byte)
    - byte 5 = {1,0} (1 = maintain state, 0 = reset to off (mk2 firmware only))
    - byte 6 = sp    (start position, 0 for entire loop)
    - byte 7 = ep    (end position, 0 for entire loop)

### Read current color - `format:  {0x01, 'r', n,0,0,       0,0, n }`

return values:

        hid_send_buf[2] = leds[ledn].r;
        hid_send_buf[3] = leds[ledn].g;
        hid_send_buf[4] = leds[ledn].b;
        hid_send_buf[5] = 0;
        hid_send_buf[6] = 0;
        hid_send_buf[7] = ledn;
        
### Playstate readback - `format: {0x01, 'S', 0,0,0,       0,0, 0 }`

return values:
    
        hid_send_buf[2] = playing;
        hid_send_buf[3] = playstart;
        hid_send_buf[4] = playend;
        hid_send_buf[5] = playcount;
        hid_send_buf[6] = playpos;
        hid_send_buf[7] = 0;
        

Examples
--------

* fade to white (255,255,255) in 5 seconds

  `{ 0x01, 'c', 0xff,0ff,0xff, 0x01,0xff, 0 }`


* fade to white (255,255,255) in 5 seconds

  `{ 0x01, 'c', 0xff,0ff,0xff, 0x01,0xff, 0 }`



