#-------------------------------------------------
#
# Project created by QtCreator 2013-04-28T13:11:31
#
#-------------------------------------------------

QT       += core gui
#CONFIG   += static

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    blink1pattern.cpp \
    todgroupbox.cpp

HEADERS  += mainwindow.h \
    blink1pattern.h \
    todgroupbox.h

FORMS    += mainwindow.ui \
    dialog.ui \
    todgroupbox.ui


# note: need to type "make lib" in blink1/commandline for this to work
BLINK1_LIB_DIR=$$PWD/../../commandline
message("BLINK1_LIB_DIR=$$BLINK1_LIB_DIR")

macx: LIBS += -L$$BLINK1_LIB_DIR -lBlink1
win32: LIBS += $$BLINK1_LIB_DIR/blink1-lib.dll

INCLUDEPATH += $$BLINK1_LIB_DIR $$BLINK1_LIB_DIR/hidapi/hidapi $$BLINK1_LIB_DIR/../hardware/firmware
DEPENDPATH += $$BLINK1_LIB_DIR


RESOURCES += \
    myresources.qrc


macx {
    MYAPPDIR=$$OUT_PWD/$${TARGET}.app/Contents/MacOS
    message( $$OUT_PWD )
    message( "MYAPPDIR = $$MYAPPDIR" )
    # FIXME: How to make this cross-platform?
    QMAKE_POST_LINK = cp -f $$BLINK1_LIB_DIR/libBlink1.dylib $$MYAPPDIR
}
# mac: note to deploy must do commandline magic of:
# % macdeployqt test2.app -verbose=2
# % mv test2.app/Contents/MacOS/libBlink1.dylib test2.app/Contents/Frameworks


win32 {
    # confusingly, these both print, why?
    #CONFIG(release, debug|release):message(*** Release build!) #will print
    #CONFIG(debug, debug|release):message(*** Debug build!) #no print

    # surely there's a better way to do this
    CONFIG(release, debug|release):  MYAPPDIR=$$OUT_PWD/release
    CONFIG(debug,   debug|release):  MYAPPDIR=$$OUT_PWD/debug

    message( "MYAPPDIR = $$MYAPPDIR" )
    #message( "CONFIG = $$CONFIG")
    # but this line doesn't work, because of forward slashes presumably
   # QMAKE_POST_LINK += COPY /Y "$$BLINK1_LIB_DIR\blink1-lib.dll" "$$MYAPPDIR"
}
# win32:
# minimum DLLs appear to be: (in "release" kit)
# Qt5Core.dll Qt5Gui.dll Qt5Widgets.dll
# icudt49.dll icuin49.dll icuuc49.dll
# D3DCompiler_43.dll libGLESv2.dll libEGL.dll
# libgcc_s_sjlj-1.dll libstdc++-6.dll  libwinpthread-1.dll
# blink1-lib.dll

#macx: LIBS += -L./HIDAPI/mac -lHIDAPI
#win32: LIBS += -L./HIDAPI/windows -lHIDAPI
#unix: !macx: LIBS += -L./HIDAPI/linux -lHIDAPI
#macx: LIBS += -framework CoreFoundation -framework IOkit
#win32: LIBS += -lSetupAPI
#unix: !macx: LIBS += -lusb-1.0


# FIXME: this lives outside the repo
include(../third-party/Qt-Color-Picker/color_widgets.pri)
