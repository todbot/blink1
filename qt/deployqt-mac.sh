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

BUILD_DIR=build-blink1control-Desktop_Qt_5_3_clang_64bit-Release

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
macdeployqt ${EXE_PATH} -qmldir=${EXE_PATH}/Contents/Resources/qml -always-overwrite
#macdeployqt ${EXE_PATH} -always-overwrite

DO_WEBKIT=0
if [ "$DO_WEBKIT" -eq 1 ] ; then
echo "Copying extra WebKit stuff..."
# except this one
# fix bug in macdeployqt wrt QtWebKit
# see: https://bugreports.qt-project.org/browse/QTBUG-35211
QT_QMAKE=`which qmake`
QT_PATH=`dirname ${QT_QMAKE}`
LIBEXEC=$EXE_PATH/Contents/libexec
mkdir -p ${LIBEXEC}
cp ${QT_PATH}/../libexec/QtWebProcess ${LIBEXEC}
fi

# copy blink1 lib
cp ../commandline/libBlink1.dylib ${EXE_PATH}/Contents/Frameworks

# zip up
pushd ${BUILD_DIR}
rm -f ../${APP_NAME}-mac.zip
zip -r ../${APP_NAME}-mac.zip ${APP_NAME}.app

echo
echo "created '${APP_NAME}-mac.zip'"

popd

