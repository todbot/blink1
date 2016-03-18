

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

Blink1Control is built using Qt-5.5 on Mac and Windows

On Windows you need to install:
- Qt-5.5 for Windows 32-bit ('msvc2013 32-bit' in Qt setup)
- MS Visual Studio 2013 (free to use)
- OpenSSL Light from http://slproweb.com/products/Win32OpenSSL.html

On Mac OS X you need to install:
- QT-5.5 for Mac
- Xcode

On Ubuntu, you need to install:
- QT-5.5 for Linux (64-bit)
- sudo apt-get install build-essential
- sudo apt-get install mesa-common-dev libgl-dev
- Note: Linux version doesn't work correctly in many weird ways

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

Blink1Control and libcurl
-------------------------
Blink1Control contains pre-build versions of the libcurl library.
It is located in:
- blink1control/libcurl-mac
- blink1contorl/libcurl-win

On Mac, we build this library statically:
0. Download libcurl sources
1. brew install openssl --universal
    which says that you get:
    LDFLAGS:  -L/usr/local/opt/openssl/lib
    CPPFLAGS: -I/usr/local/opt/openssl/include
2. tar xvzf curl-7.43.0.tar.gz && cd curl-7.43.0
3. CFLAGS=-mmacosx-version-min=10.8 ./configure --disable-shared --with-darwinssl --prefix=${HOME}/projects/curl/libcurl-mac
4. make && make install   # (puts it in 'tst1' directory)
5. rm -rf ~/projects/blink1/qt/blink1control/libcurl-mac
5. mv ${HOME}/projects/curl/libcurl-mac  ~/projects/blink1/qt/blink1control


Location of Blink1Control config file:
----------------------
On Windows:
- %APPDATA%\ThingM\Blink1Control.ini

On Mac OS X:
- $HOME/.config/ThingM/Blink1Control.ini

