

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

Blink1Control is built using Qt-5.3 on Mac and Windows

On Windows you need to install:
- Qt-5.3 for Windows 32-bit (VS 2013)
- MS Visual Studio 2013 (free to use)
- OpenSSL Light from http://slproweb.com/products/Win32OpenSSL.html

On Mac OS X you need to install:
- QT-5.3 for Mac
- Xcode

On Ubuntu, you need to install:
- QT-5.3 for Linux (64-bit)
- sudo apt-get install build-essential
- sudo apt-get install mesa-common-dev libgl-dev

To build, open up QTCreator and press "build".

Be sure to remove the `build-blink1control-Desktop_Qt_5_3...`
directory before building to completely rebuild all files.

Once a release version is built.  You can create deployable zip
bundles with:
- ./deploy-mac.sh
- ./deploy-win.sh

You may need to edit the variables at the top of those scripts to
match your Qt install.

To build a Windows installer, get Inno Setup
(http://www.jrsoftware.org/isinfo.php ) and use
`Blink1Control.iss` setup file

To build a Mac OS X DMG, get Aolor DMG Maker
(http://dmgmakermac.aolor.com/ ) and use
`Blink1Control.dmgdoc` setup file.



Location of Blink1Control config file:
----------------------
On Windows:
- %APPDATA%\ThingM\Blink1Control.ini

On Mac OS X:
- $HOME/.config/ThingM/Blink1Control.ini

