#!/bin/sh
#
#
#
#

APP_NAME=Blink1Control.app

BUILD_DIR=build-blink1control-Desktop_Qt_5_2_1_clang_64bit-Release

EXE_PATH=${BUILD_DIR}/${APP_NAME}


# 
#macdeployqt ${APP_NAME} -qmldir=./Blink1Control.app/Contents/Resources/qml -verbose=2
macdeployqt ${EXE_PATH} -qmldir=${EXE_PATH}/Contents/Resources/qml

# copy blink1 lib
cp ../commandline/libBlink1.dylib ${EXE_PATH}/Contents/Frameworks


# zip up
pushd ${BUILD_DIR}
zip -r ../${APP_NAME}-mac.zip ${APP_NAME}
popd

