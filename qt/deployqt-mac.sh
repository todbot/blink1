#!/bin/sh
#
# Before running this, be sure to:
# 1. Select "Release" build in QtComposer 
# 2. Build Blink1Control
# 3. Do QC Tests
# 4. Type "./deployqt-mac.sh" (i.e. must be in this dir)
# 5. Resulting zipped up app will be this directory as "Blink1Control-mac.zip"
#
#

APP_NAME=Blink1Control

#BUILD_DIR=build-blink1control-Desktop_Qt_5_3_clang_64bit-Release
BUILD_DIR=./build-blink1control-Desktop_Qt_5_4_clang_64bit-Release

QT_BIN_PATH=${HOME}/Qt/5.4/clang_64/bin
export PATH=${QT_BIN_PATH}:${PATH}

# used passed in arg instead of default above
if [ -n "$1" ] ; then
    BUILD_DIR=$1
fi

EXE_PATH=${BUILD_DIR}/${APP_NAME}.app

if [ ! -d "$EXE_PATH" ] ; then
  echo "Path not found: ${EXE_PATH}"
  exit
else 
  echo "Deploying '$EXE_PATH'"
  echo
fi

# macdeployqt copies all the needed Qt libs
pushd ${BUILD_DIR}
macdeployqt ${APP_NAME}.app -qmldir=./${APP_NAME}.app/Contents/Resources/qml -always-overwrite -verbose=2
#macdeployqt ${EXE_PATH} -qmldir=${EXE_PATH}/Contents/Resources/qml -always-overwrite
#macdeployqt ${EXE_PATH} -always-overwrite
popd

# copy blink1 lib
mkdir -p ${EXE_PATH}/Contents/Frameworks
cp ../commandline/libBlink1.dylib ${EXE_PATH}/Contents/Frameworks

# zip up
echo "zipping up ${APP_NAME}"
pushd ${BUILD_DIR}
rm -f ../${APP_NAME}-mac.zip
zip -q -r ../${APP_NAME}-mac.zip ${APP_NAME}.app

echo
echo "created '${APP_NAME}-mac.zip'"

popd

