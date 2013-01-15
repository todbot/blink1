Contributed Linux Scripts
=========================

This folder contains Linux scripts that were contributed by others.

blink1-webcam-busylight.sh
--------------------------
Automatically change a Blink(1)'s color based on a USB Webcam's camera or audio use, turning the Blink(1) into a handy "Busy Light" to notify others that you are on Skype, Google Chat or other similar services.

cheerlights-to-blink1.pl 
------------------------
Set blink1 to current Cheerlights color.  Uses Thingspeak.com. 

gmail-to-blink1.sh
------------------
Blinks your Blink(1) when you have unread Gmail!

Checks your Gmail account and blinks the Blink(1) with a definable flash of color (default: bright blue) if you have any unread mail.

Additionally, adds a quick initial burst of a second color (default: red) before the regular flash to indicate when you have more unread mail than a definable amount (default: 10 but can also be disabled)

load-to-blink1.sh
------------------
Automatically change a Blink(1)'s color based on system load.

Sets the Blink(1) to user definable colors based on user definable thresholds for loads 1, 5 and 15. In order of precedence, the defaults (which you probably want to change to something appropriate for the system) are:

* Below thresholds (Blue)
* >2.00 Load 15 (Red)
* >3.00 Load 5 (Orange)
* >4.00 Load 1 (Yellow)

travisci-to-blink1.sh
---------------------
Shows the status of your travis-ci.org project on your Blink(1).

Polls the status of any public project on travis-ci.org and sets your Blink(1) to red, green, or yellow to match.

-eof-
