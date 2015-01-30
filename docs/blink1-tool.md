
blink1-tool command-line API
============================

The below is created from capturing the output of "blink1-tool" with no arguments.

For example usage, see [blink1-tool-tips.md](blink1-tool-tips.md)

```
Usage:
  blink1-tool <cmd> [options]
where <cmd> is one of:
  --list                      List connected blink(1) devices
  --rgb=<red>,<green>,<blue>  Fade to RGB value
  --rgb=[#]RRGGBB             Fade to RGB value, as hex color code
  --hsb=<hue>,<sat>,<bri>     Fade to HSB value
  --blink <numtimes>          Blink on/off (use --rgb to blink a color)
  --on | --white              Turn blink(1) full-on white
  --off                       Turn blink(1) off
  --red                       Turn blink(1) red
  --green                     Turn blink(1) green
  --blue                      Turn blink(1) blue
  --cyan                      Turn blink(1) cyan (green + blue)
  --magenta                   Turn blink(1) magenta (red + blue)
  --yellow                    Turn blink(1) yellow (red + green)
  --rgbread                   Read last RGB color sent (post gamma-correction)
  --setpattline <pos>         Write pattern RGB val at pos (--rgb/hsb to set)
  --getpattline <pos>         Read pattern RGB value at pos
  --savepattern               Save color pattern to flash (mk2)
  --play <1/0,pos>            Start playing color sequence (at pos)
  --play <1/0,start,end,cnt>  Playing color sequence sub-loop (mk2)
  --servertickle <1/0>[,1/0]  Turn on/off servertickle (w/on/off, uses -t msec)
  --chase                     Multi-LED chase effect (uses --led & --rgb)
  --random, --random=<num>    Flash a number of random colors
  --glimmer, --glimmer=<num>  Glimmer a color with --rgb (num times)
 Nerd functions: (not used normally)
  --eeread <addr>             Read an EEPROM byte from blink(1)
  --eewrite <addr>,<val>      Write an EEPROM byte to blink(1)
  --fwversion                 Display blink(1) firmware version
  --version                   Display blink1-tool version info
and [options] are:
  -d dNums --id all|deviceIds Use these blink(1) ids (from --list)
  -g -nogamma                 Disable autogamma correction
  -m ms,   --millis=millis    Set millisecs for color fading (default 300)
  -q, --quiet                 Mutes all stdout output (supercedes --verbose)
  -t ms,   --delay=millis     Set millisecs between events (default 500)
  -l <led>, --led=<led>       Set which RGB LED in a blink(1) mk2 to use
  -v, --verbose               verbose debugging msgs

Examples
  blink1-tool -m 100 --rgb=255,0,255    # fade to #FF00FF in 0.1 seconds
  blink1-tool -t 2000 --random=100      # every 2 seconds new random color
  blink1-tool --rgb 0xff,0,00 --blink 3 # blink red 3 times
  blink1-tool --rgb FF9900              # make blink1 pumpkin orange
  blink1-tool --rgb #FF9900             # make blink1 pumpkin orange

Notes
 - To blink a color with specific timing, specify 'blink' command last:
   blink1-tool -t 200 -m 100 --rgb ff00ff --blink 5
 - If using several blink(1)s, use '-d all' or '-d 0,2' to select 1st,3rd:
   blink1-tool -d all -t 50 -m 50 -rgb 00ff00 --blink 10

    
        
```


