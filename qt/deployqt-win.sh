#!/bin/sh
#
# deployqt-win.sh -- Take a release build of Blink1Control 
#                    and create a runnable application
#  
# Before running this, be sure to:
# 1. Select "Release" build in QtComposer 
# 2. Build Blink1Control
# 3. Do QC Tests
# 4. Type "./deployqt-win.sh" (i.e. must be in this dir in a MinGW shell)
# 5. Resulting zipped up app will be this directory as "Blink1Control-win.zip"
#
# Wow this is such a hack.  
# Is this really what one must do to make executables on Windows?
# (though MacOSX is not much better)
#
# 2014 Tod E. Kurt, http://thingm.com/
#
#set -x # echo on

# BEGIN CONFIGURATION

# Where "windeployqt.exe" lives (and the mingw libs)
#QT_BIN_PATH=/c/qt/Qt5.2.1/5.2.1/mingw48_32/bin
#QT_BIN_PATH=/c/Qt/5.3/mingw482_32/bin
#QT_BIN_PATH=/c/qt/Qt5.3.1/5.3/msvc2013/bin
#QT_BIN_PATH=/c/qt/5.4/msvc2013/bin
#QT_DIR=/c/qt/5.5/msvc2013
QT_DIR=/c/qt/5.5/msvc2013
QT_BIN_PATH=${QT_DIR}/bin

# Where Visual Studio installation directory needed by "windeployqt.exe"
#export VCINSTALLDIR=/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 12.0/VC
export VCINSTALLDIR=/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 12.0/VC/

# (paths are from the soon-to-be-created APP_DIR
# path to built application executable
#BUILD_DIR=build-blink1control-Desktop_Qt_5_2_1_MinGW_32bit-Release/release
#BUILD_DIR=build-blink1control-Desktop_Qt_5_3_MinGW_32bit-Release/release
#BUILD_DIR=build-blink1control-Desktop_Qt_5_3_MSVC2013_32bit-Release/release
#BUILD_DIR=build-blink1control-Desktop_Qt_5_4_MSVC2013_32bit-Release/release
#BUILD_DIR=build-blink1control-Desktop_Qt_5_4_MSVC2013_32bit-Release/release
#BUILD_DIR=build-blink1control-Desktop_Qt_5_4_2_MSVC2013_32bit-Release/release
#BUILD_DIR=build-blink1control-Desktop_Qt_5_5_1_MSVC2013_32bit-Release/release
BUILD_DIR=build-blink1control-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug/debug

# END CONFIGURATION

# name of resulting directory you want, containing the application
APP_DIR=Blink1Control
EXE_NAME=Blink1Control.exe

# where the source code lives (and the qml, and the help, and the readme)
SRC_DIR="../../blink1control"

# used passed in arg instead of default above
if [ -n "$1" ] ; then
    BUILD_DIR=$1
fi

EXE_PATH="${BUILD_DIR}/${EXE_NAME}"

if [ ! -e $EXE_PATH ] ; then 
    echo "file not found: ${EXE_PATH}"
    exit
fi

echo "Deploying '$EXE_PATH'"
echo

# where QML files live
QML_DIR=${SRC_DIR}/qml
# where Help files live
HELP_DIR=${SRC_DIR}/help

# Location of blnik1-lib.dll (do "make lib" in that dir first to get it)
BLINK1_LIB_PATH=../../../commandline/blink1-lib.dll

export PATH=${PATH}:${QT_BIN_PATH}

# make the place where we're going to put the whole app
rm -rf windeploy
mkdir windeploy/${APP_DIR}
pushd windeploy/${APP_DIR}

# copy the built executable
cp ../../${EXE_PATH} .

# copy readme
cp ${SRC_DIR}/README.txt .

# copy blink1-lib
cp ${BLINK1_LIB_PATH} .

# copy qml files  (seems like windeployqt should do this, but it doesn't do it, correctly)
cp -r ${QML_DIR} .

# copy help files (this is a dep in blink1control.pro, why isn't it taken care of?)
mkdir help
cp -r ${HELP_DIR} help 

# don't do for MSVC
# copy mingw libs (seems like windeployqt should do this too)
#cp ${QT_BIN_PATH}/lib*dll .

# copy MSVCRT DLLS
cp /c/windows/syswow64/{msvcp120,msvcr120}.dll .

# copy SSL DLLS  
# get it from http://slproweb.com/products/Win32OpenSSL.html
cp /c/OpenSSL-Win32/bin/libeay32.dll .
cp /c/OpenSSL-Win32/bin/ssleay32.dll .

# copy CURL DLLS  (FIXME: this seems to include SSL dlls yeah?)
cp ${SRC_DIR}/libcurl-win/bin/*dll .

#WINDEPLOYQT_OPTS=" --qmldir ${QML_DIR}"
WINDEPLOYQT_OPTS+=" --no-translations"
WINDEPLOYQT_OPTS+=" --no-webkit2"
WINDEPLOYQT_OPTS+=" --no-webkit"
#WINDEPLOYQT_OPTS+=" --verbose 2"
#WINDEPLOYQT_OPTS+=" --libdir ../../../commandline/blink1-lib.dll"
#WINDEPLOYQT_OPTS+=" --libdir ${QT_BIN_PATH}"
#WINDEPLOYQT_OPTS+=" --no-sql --no-system-d3d-compiler"
# windeploy to get rest of Qt dependencies
${QT_BIN_PATH}/windeployqt ${WINDEPLOYQT_OPTS}  Blink1Control.exe


#exit 0   # uncomment for faster testing 

# Build a zip bundle
echo "zipping up ${APP_DIR}"
cd ..
rm -f ../${APP_DIR}-win.zip
zip -q -r ../${APP_DIR}-win.zip ${APP_DIR}

echo
echo "Done. Created '${APP_DIR}-win.zip'"

popd


