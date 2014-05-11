#!/bin/sh
#
# deployqt-win.sh --
#  
# Wow this is such a hack.  
# Is this really what one must do to make executables on Windows?
# (though MacOSX is not much better)
#
# 2014 Tod E. Kurt, http://thingm.com/
#

# name of resulting directory you want, containing the application
APP_DIR=windeploy

# path to built application executable
EXE_PATH=./build-blink1control-Desktop_Qt_5_2_1_MinGW_32bit-Release/release/Blink1Control.exe 

# where QML files live
QML_DIR=./blink1control/qml

# Where "windeployqt.exe" lives (and the mingw libs)
QT_BIN_PATH=/c/qt/Qt5.2.1/5.2.1/mingw48_32/bin

# location of blnik1-lib.dll (do "make lib" in that dir first to get it)
BLINK1_LIB_PATH=../commandline/blink1-lib.dll

# make the place where we're going to put the whole app
mkdir windeployqt
cd windeployqt

if [ -e $EXE_PATH ] ; then 
    # copy the built executable
    cp ${EXE_PATH} .

    # copy blink1-lib
    cp ${BLINK1_LIB_PATH} .

    # copy qml files  (seems like windeployqt should do this, but it doesn't)
    cp -r ${QML_DIR} .

    # copy mingw libs (seems like windeployqt should do this too)
    cp ${QT_BIN_PATH}/lib*dll .

    # windeploy to get rest of Qt dependencies
    ${QT_BIN_PATH}/windeployqt Blink1Control.exe

    # Build a zip bundle
    # zip -r ${APP_DIR}.zip ${APP_DIR}
else
    echo "file not found: ${EXE_PATH}"
fi

