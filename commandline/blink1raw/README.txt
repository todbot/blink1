

blink1raw -- use HIDRAW to talk to blink(1)
--------------------------------------------
2012 - Frederick Roeber 

This tiny program is great for restricted environments like during machine installs


Some sample command lines:
% blink1 /dev/hidraw* % _ @1:255,0,0,1 @2:255,0,0,50 @3:255,100,0,1
@4:255,100,0,50 +1

Arguments, in order:
 /dev/hidraw* -- this gets shell-expanded into a list of devices.
       The program will open each in turn, check if it's a blink(1), and
       if so, it becomes the new target.  If you have one such device,
       this is an easy way of finding it.  If you have multiple, whichever
       is last will be the device.

 % -- clear all steps
 @1: set step 1 to be "fade to red in 1 cs"  1/100 of a second is
short, so it flips to red.
 @2: set step 2 to be "fade to red in 50 cs" which, since it's
already red, is "hold red for 50 cs"
 @3: set step 3 to be "fade to yellow in 1 cs" i.e. flip to yellow
 @4: set step 4 to be "fade to yellow in 50 cs" i.e. hold yellow for
another half second.
 +1 -- begin playing at step 1

This is the pattern used when a machine need its key inserted.  (I
haven't assigned others yet, but I'm imagining that every problem will
have its own pattern, so I can tell at a glance what's going on.)


% blink1 /dev/hidraw* =255,0,255,50

This just sets the found device to be green.  It does this when the
boot key has been detected and accepted.


% blink1 /dev/hidraw* _ % @1:0,255,0,100 @2:0,0,0,100 @3:0,255,0,100
@4:0,0,0,100 @5:0,255,0,100 @6:0,0,0,100 @7:0,0,0,1000 +1

This throbs its green three times (100 cs, i.e. 1 second, per fade-up
or -down), then holds black for 10 seconds.  This happens when the
machine is happy and the key can be removed.  The driver script waits
six seconds, then issues a

% blink1 /dev/hidraw* _100 %

which fades it off over 1 second, and clears the program.

