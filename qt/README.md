

Blink1Control in Qt
============

Blink1Control is the primary user-level application for controlling blink(1).
It is an event-based system, where events on your computer or on the Net
trigger color patterns on the blink(1).

It can receive events from:
- IFTTT
- various email services using IMAP/POP/Gmail
- execute scripts or check files 
- hardware monitors like CPU, battery, RAM

This verison of Blink1Control supercedes the Mac-specific and Windows-specific
versions in the "mac" and "win" directories.


Building and Deploying the Blink1Contol Qt application
-----------------------------------------------

Blink1Control is built using Qt-5.3 on Mac and Windows (MinGW
version).

Once a release version is build.  You can create deployable zip
bundles with:
- deploy-mac.sh
- deploy-win.sh

You may need to edit the variables at the top of those scripts to
match your Qt install.


Location of Blink1Control config file:
----------------------
On Windows:
- %APPDATA%\ThingM\Blink1Control.ini

On Mac OS X:
- $HOME/.config/ThingM/Blink1Control.ini

