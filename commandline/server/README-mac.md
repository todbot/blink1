
(note: this is stale, do not use)


Making it a service on Mac OS X
-------------------------------

to start:
% launchctl load -w ./com.thingm.blink1.plist
% launchctl start com.thingm.blink1

to stop:
% launchctl stop com.thingm.blink1
% launchctl unload ./com.thingm.blink1.plist
