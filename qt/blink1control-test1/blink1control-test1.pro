#-------------------------------------------------
#
# blink1control-test1 --
#  A simple demonstration of integrating the C
#  blink1-lib into Qt.
#
# 2013, Tod E. Kurt, http://thingm.com/
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blink1control-test1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#
# Build the blink(1) shared library before trying to build this project.
# Go into blink1/commandline directory and do: "make lib".
# The result will be "blink1-lib.dll" (windows) or "libBlink1.dylib" (macx)
# You will then need to copy the lib into the executable directory.
#
BLINK1_LIB_DIR=$$PWD/../../commandline
message("BLINK1_LIB_DIR=$$BLINK1_LIB_DIR")

macx: LIBS += -L$$BLINK1_LIB_DIR -lBlink1
win32: LIBS += $$BLINK1_LIB_DIR/blink1-lib.dll

INCLUDEPATH += $$BLINK1_LIB_DIR $$BLINK1_LIB_DIR/hidapi/hidapi $$BLINK1_LIB_DIR/../hardware/firmware
DEPENDPATH += $$BLINK1_LIB_DIR

macx: {
    MYAPPDIR=$$OUT_PWD/$${TARGET}.app/Contents/MacOS
    message( "copying libBlink1.dylib to $$MYAPPDIR" )
    # FIXME: How to make this cross-platform?
    QMAKE_POST_LINK = cp -f $$BLINK1_LIB_DIR/libBlink1.dylib $$MYAPPDIR
}

win32 {
    MYAPPDIR=$$OUT_PWD/debug
    CP_CMD="c:/MinGW/msys/1.0/bin/cp -f"
    message( "copying blink1-lib.dll to $$MYAPPDIR" )
    QMAKE_POST_LINK = "$$CP_CMD $${BLINK1_LIB_DIR}/blink1-lib.dll $${MYAPPDIR}"
    #QMAKE_POST_LINK = "COPY /Y $${BLINK1_LIB_DIR}/blink1-lib.dll $${MYAPPDIR}"
}
