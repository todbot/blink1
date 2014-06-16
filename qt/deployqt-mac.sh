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

BUILD_DIR=build-blink1control-Desktop_Qt_5_2_1_clang_64bit-Release

EXE_PATH=${BUILD_DIR}/${APP_NAME}.app


# 
#macdeployqt ${APP_NAME} -qmldir=./Blink1Control.app/Contents/Resources/qml -verbose=2
macdeployqt ${EXE_PATH} -qmldir=${EXE_PATH}/Contents/Resources/qml

# copy blink1 lib
cp ../commandline/libBlink1.dylib ${EXE_PATH}/Contents/Frameworks


# zip up
rm -f ../${APP_NAME}-mac.zip
pushd ${BUILD_DIR}
zip -r ../${APP_NAME}-mac.zip ${APP_NAME}.app
popd

