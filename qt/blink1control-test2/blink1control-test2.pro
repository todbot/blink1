#-------------------------------------------------
#
# Project created by QtCreator 2013-04-28T13:11:31
#
#-------------------------------------------------

QT       += core gui

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

#unix|win32: LIBS += -L$$PWD/../../blink1/commandline -lBlink1
LIBS += -L$$PWD/../../commandline -lBlink1
INCLUDEPATH += $$BLINK1_LIB_DIR $$BLINK1_LIB_DIR/hidapi/hidapi $$BLINK1_LIB_DIR/../hardware/firmware
DEPENDPATH += $$BLINk1_LIB_DIR

#macx: LIBS += -L./HIDAPI/mac -lHIDAPI
#win32: LIBS += -L./HIDAPI/windows -lHIDAPI
#unix: !macx: LIBS += -L./HIDAPI/linux -lHIDAPI
#macx: LIBS += -framework CoreFoundation -framework IOkit
#win32: LIBS += -lSetupAPI
#unix: !macx: LIBS += -lusb-1.0


RESOURCES += \
    myresources.qrc


macx {
    MYAPPDIR=$$OUT_PWD/$${TARGET}.app/Contents/MacOS
    message( $$OUT_PWD )
    message( "MYAPPDIR = $$MYAPPDIR" )
    # FIXME: How to make this cross-platform?
    QMAKE_POST_LINK = cp -f $$BLINK1_LIB_DIR/libBlink1.dylib $$MYAPPDIR
}

# note to deploy must do commandline magic of:
# % macdeployqt test2.app -verbose=2
# % mv test2.app/Contents/MacOS/libBlink1.dylib test2.app/Contents/Frameworks

# FIXME: this lives outside the repo
include(../third-party/Qt-Color-Picker/color_widgets.pri)
