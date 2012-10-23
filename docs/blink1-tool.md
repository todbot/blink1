
blink1-tool command-line API
============================

The below is created from capturing the output of "blink1-tool" with no arguments.
    
    Usage: 
      blink1-tool <cmd> [options]
    where <cmd> is one of:
      --hidread                   Read a blink(1) USB HID GetFeature report 
      --hidwrite <listofbytes>    Write a blink(1) USB HID SetFeature report 
      --eeread <addr>             Read an EEPROM byte from blink(1)
      --eewrite <addr>,<val>      Write an EEPROM byte to blink(1) 
      --blink <numtimes>          Blink on/off 
      --random <numtimes>         Flash a number of random colors 
      --rgb <red>,<green>,<blue>  Fade to RGB value
      --savergb <r>,<g>,<b>,<pos> Write pattern RGB value at pos
      --readrgb <pos>             Read pattern RGB value at pos
      --servertickle <on/off>     Turn on/off servertickle (uses -t msec) 
      --on                        Turn blink(1) full-on white 
      --off                       Turn blink(1) off 
      --list                      List connected blink(1) devices 
      --version                   Display blink(1) firmware version 
    and [options] are: 
      -g -nogamma                 Disable autogamma correction
      -d dNums --id all|deviceIds Use these blink(1) ids (from --list) 
    //--serial <num>              Connect to blink(1) by its serial number 
      -m ms,   --miilis=millis    Set millisecs for color fading (default 300)
      -t ms,   --delay=millis     Set millisecs between events (default 500)
      --vid=vid --pid=pid         Specifcy alternate USB VID & PID
      -v, --verbose               verbose debugging msgs
    
    Examples 
      blink1-tool -m 100 --rgb 255,0,255   # fade to #FF00FF in 0.1 seconds 
      blink1-tool -random 100              # fade to 100 random colors
    
        



