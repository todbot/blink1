blink(1) HID commands
=====================

Communication with blink(1) is done via USB HID Feature Reports.
blink(1) currently responds to one feature report, with a report id of 0x01.
The blink(1) mk3 or greater also responds to feature report 0x02
for certain advanced features.
This document shows the formats of the feature report data packet sent to blink(1).


## Command Summary ##

    - Fade to RGB color       format: { 1, 'c', r,g,b,     th,tl, n }
    - Set RGB color now       format: { 1, 'n', r,g,b,       0,0, n } (*)
    - Read current RGB color  format: { 1, 'r', n,0,0,       0,0, n } (2)
    - Serverdown tickle/off   format: { 1, 'D', on,th,tl,  st,sp,ep } (*)
    - PlayLoop                format: { 1, 'p', on,sp,ep,c,    0, 0 } (2)
    - Playstate readback      format: { 1, 'S', 0,0,0,       0,0, 0 } (2)
    - Set color pattern line  format: { 1, 'P', r,g,b,     th,tl, p }
    - Save color patterns     format: { 1, 'W', 0,0,0,       0,0, 0 } (2)
    - Read color pattern line format: { 1, 'R', 0,0,0,       0,0, p }
    - Set ledn                format: { 1, 'l', n,0,0,       0,0, 0 } (2+)
    - Read EEPROM location    format: { 1, 'e', ad,0,0,      0,0, 0 } (1)
    - Write EEPROM location   format: { 1, 'E', ad,v,0,      0,0, 0 } (1)
    - Get version             format: { 1, 'v', 0,0,0,       0,0, 0 }
    - Test command            format: { 1, '!', 0,0,0,       0,0, 0 }
    - Write 50-byte note      format: { 2, 'F', noteid, data0 ... data99 } (3)
    - Read  50-byte note      format: { 2, 'f', noteid, data0 ... data99 } (3)
    - Go to bootloader        format: { 1, 'G', 'o','B','o','o','t',0 } (3)
    - Lock go to bootload     format: { 2','L'  'o','c','k','B','o','o','t','l','o','a','d'} (3)
    - Set startup params      format: { 1, 'B', bootmode, playstart,playend,playcnt,0,0} (3)
    - Get startup params      format: { 1, 'b', 0,0,0, 0,0,0        } (3)
    - Server mode tickle      format: { 1, 'D', {1/0},th,tl, {1,0},sp, ep }
    - Get chip unique id      format: { 2, 'U', 0 } (3)

where:

    r,g,b = 24-bit RGB color
    
        n = which LED to address: 0=all, 1=led#1, 2=led#2, etc. (mk2 only)
       th = (fadetimeMillis/10) >> 8
       tl = (fadetimeMillis/10) & 0xff
       on = 1 or 0, indicating on/off, play/pause, etc.
       st = 1 or 0, indicating on/off, maintain state or clear state (mk2 only)
        p = position in list, starts at 0, goes to patt_max-1 
       sp = start loop position (0 - patt_max-1)
       ep = end loop position (1 - patt_max)
        c = count of times to repeat looping
       ad = address starting at 1
        v = arbitrary value
        1 = report id (must always be present)

      (*) some arguments for command is only available in mk2 or greater devices
      (3) mk3 devices or greater
      (2) mk2 devices or greater
      (1) mk1 devices or greater


Protocol
--------

The blink(1) feature report packet is 8 bytes long.
The structure of that packet is roughly:

    - byte 0 = report_id (0x01)
    - byte 1 = command action ('c' = fade to rgb, 'v' get firmware version, etc.)
    - byte 2 = cmd arg 0 (e.g. red)
    - byte 3 = cmd arg 1 (e.g. green)
    - byte 4 = cmd arg 2 (e.g. blue)
    - byte 5 = cmd arg 3 (e.g. th)
    - byte 6 = cmd arg 4 (e.g. tl)
    - byte 7 = cmd arg 5 (e.g. ledn)

Note:

- The command action is the ascii value of the letter given (e.g. 'c' = 0x63)
- Any command with unused cmd args should set them to zero.
- On mk2 devices, gamma-correction of R,G,B values is done on the device,
  while on mk1 devices, the gamma correction is done in blink1-lib
- The preferred implementation of of the protocol is in blink1-lib.{c,h}
- patt_max = 12 (mk1), patt_max = 32 (mk2+)

The most common command is "fade to RGB", which has the form:

    - byte 0 = 0x01  (report_id )
    - byte 1 = 'c'   (command "fade to rgb")
    - byte 2 = red value
    - byte 3 = green value
    - byte 4 = blue value
    - byte 5 = th    (fadeMillis/10 high byte)
    - byte 6 = tl    (fadeMillis/10 low byte)
    - byte 7 = ledn  (unused on mk1, 0=all on mk2) 


Commands
--------

### Fade To RGB - `format: {0x01, 'c', r,g,b,    th,tl, n }`

This is the most common command sent to a blink(1).
It lights up the blink(1) with the specified RGB color,
fading to that color over a specified number of milliseconds.

For a given fade time "fadeMillis", the "th" and "tl" values are computed by:
    th = (fadetimeMillis/10) >> 8
    tl = (fadetimeMillis/10) & 0xff

The R,G,B, values are the 0-255 R,G,B components of the color to send.

This command does not produce a return value.

    - byte 0 = 0x01  (report_id)
    - byte 1 = 'c'   (command "fade to rgb")
    - byte 2 = red value
    - byte 3 = green value
    - byte 4 = blue value
    - byte 5 = th    (fadeMillis/10 high byte)
    - byte 6 = tl    (fadeMillis/10 low byte)
    - byte 7 = ledn  (unused on mk1, ledn on mk2+)

### Servertickle - `format: {0x01, 'D', on,th,tl,  st,0, 0 }`

This command does not produce a return value.

    - byte 0 = 0x01  (report_id)
    - byte 1 = 'D'   (command "servertickle")
    - byte 2 = {1,0} (1 = enable servertickle, 0 = disable servertickle)
    - byte 3 = th    (timeout/10 high byte)
    - byte 4 = tl    (timeout/10 low byte)
    - byte 5 = {1,0} (1 = maintain state, 0 = reset to off (mk2 firmware only))
    - byte 6 = sp    (start position, 0 for entire loop)
    - byte 7 = ep    (end position, 0 for entire loop)

### Read Color Pattern Line - `format: { 1, 'R', 0,0,0,       0,0, p }`

return values:

    hid_send_buf[0] = 0x01 (report_id)
    hid_send_buf[1] = 'R'
    hid_send_buf[2] = r;
    hid_send_buf[3] = g;
    hid_send_buf[4] = b;
    hid_send_buf[5] = th;
    hid_send_buf[6] = tl;
    hid_send_buf[7] = pos;


### Read current color - `format:  {0x01, 'r', 0,0,0,       0,0, n }` 

return values:

    hid_send_buf[0] = 0x01 (report_id)
    hid_send_buf[1] = 'r'
    hid_send_buf[2] = leds[ledn].r;
    hid_send_buf[3] = leds[ledn].g;
    hid_send_buf[4] = leds[ledn].b;
    hid_send_buf[5] = 0;
    hid_send_buf[6] = 0;
    hid_send_buf[7] = ledn;

where ledn is 0 (one led) or 1 (the other led).

### Playstate readback - `format: {0x01, 'S', 0,0,0,       0,0, 0 }` 

return values:
    
    hid_send_buf[0] = 0x01 (report_id)
    hid_send_buf[1] = 'S'
    hid_send_buf[2] = playing;
    hid_send_buf[3] = playstart;
    hid_send_buf[4] = playend;
    hid_send_buf[5] = playcount;
    hid_send_buf[6] = playpos;
    hid_send_buf[7] = 0;
        


Examples
--------

* Fade to white (255,255,255) in 5 seconds

  `{ 0x01, 'c', 0xff,0ff,0xff, 0x01,0xff, 0 }`


* Fade to white (255,255,255) in 5 seconds

  `{ 0x01, 'c', 0xff,0ff,0xff, 0x01,0xff, 0 }`

* Read color pattern line #5

  `{ 0x01, 'R', 0,0,0,  0,0, 5}`

  example response (#FF00FF @ 500msec)

  `{ 0x01, 'R', 0xff,0x00,0xff,  0x00,0x32, 5 }`


