Blink1Server -- Mac OS X application for controlling blink(1) USB RGB LED
============


Local webserver URL API:

- /
  -- docroot for static HTML files hosted in "html"

- /blink1/

  -- general status page for blink(1) devices connected to system
  -- returns:
   
- /blink1/list/
  -- list blink(1)s
  -- response: list of blink(1) device ids
  -- response example: { "status" : "ok", 
                         "blink1_serialnums" : [ "FE0023","FD12CC" ] }
  
- /blink1/fadeToRGB/
  -- params
      'rgb' = rgb hex color code
      'time' = time in float seconds
      'serialnum' = blink1 serial number to use, if omitted, use first blink(1)
  -- returns:
  -- example: ?rgb=#FF33dd&time=1.2
  
  
- /blink1/addPattern/