#-------------------------------------------------
#
#-------------------------------------------------

QT       += core gui widgets network

folder_01.source = qml/qml
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

TARGET = Blink1Control#test2
VERSION = 0.9.8
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    blink1pattern.cpp \
    blink1input.cpp \
    datainput.cpp \
    bigbuttons.cpp \
    email.cpp \
    simplecrypt.cpp \
    hardwaremonitor.cpp \
    Cursorshapearea.cpp

HEADERS  += mainwindow.h \
    blink1pattern.h \
    blink1input.h \
    datainput.h \
    bigbuttons.h \
    email.h \
    simplecrypt.h \
    hardwaremonitor.h \
    Cursorshapearea.h

#FORMS    += mainwindow.ui

RESOURCES += \
    myresources.qrc


# for blink1-lib
# note: need to type "make lib" in blink1/commandline beforehand
BLINK1_LIB_DIR=$$PWD/../../commandline
message("BLINK1_LIB_DIR=$$BLINK1_LIB_DIR")

macx: LIBS += -L$$BLINK1_LIB_DIR -lBlink1
win32: LIBS += $$BLINK1_LIB_DIR/blink1-lib.dll

QMAKE_CXXFLAGS += -DUSE_HIDAPI
#QMAKE_CFLAGS += -DUSE_HIDAPI
INCLUDEPATH += $$BLINK1_LIB_DIR $$BLINK1_LIB_DIR/hidapi/hidapi $$BLINK1_LIB_DIR/../hardware/firmware
DEPENDPATH += $$BLINK1_LIB_DIR

# for tasteful-server
##INCLUDEPATH += ../third-party/tasteful-server/include
##macx: LIBS += -L../third-party/tasteful-server -lTastefulServer


macx {
    MYAPPDIR=$$OUT_PWD/$${TARGET}.app/Contents/MacOS
    message( "MYAPPDIR = $$MYAPPDIR" )
    # FIXME: How to make this cross-platform?
    # FIXME: libBlink1 should be in .app/Contents/Frameworks now
    QMAKE_POST_LINK = cp -f $$BLINK1_LIB_DIR/libBlink1.dylib $$MYAPPDIR
}

win32 {
    # confusingly, these both get run, why?
    # surely there's a better way to do this
    CONFIG(release, debug|release):  MYAPPDIR=$$OUT_PWD/release
    CONFIG(debug,   debug|release):  MYAPPDIR=$$OUT_PWD/debug

    message( "MYAPPDIR = $$MYAPPDIR" )
    # but this line doesn't work, because of forward slashes presumably
    #QMAKE_POST_LINK += COPY /Y "$$BLINK1_LIB_DIR\blink1-lib.dll" "$$MYAPPDIR"
}

# win32:
# minimum DLLs appear to be: (in "release" kit)
# Qt5Core.dll Qt5Gui.dll Qt5Widgets.dll
# icudt49.dll icuin49.dll icuuc49.dll
# D3DCompiler_43.dll libGLESv2.dll libEGL.dll
# libgcc_s_sjlj-1.dll libstdc++-6.dll  libwinpthread-1.dll
# blink1-lib.dll

# some hints from a hidapi app
#macx: LIBS += -L./HIDAPI/mac -lHIDAPI
#win32: LIBS += -L./HIDAPI/windows -lHIDAPI
#unix: !macx: LIBS += -L./HIDAPI/linux -lHIDAPI
#macx: LIBS += -framework CoreFoundation -framework IOkit
#win32: LIBS += -lSetupAPI
#unix: !macx: LIBS += -lusb-1.0


#include(../third-party/Qt-Color-Picker/color_widgets.pri)

# Fervor autoupdater
#!include("../third-party/fervor/Fervor.pri") {
#    error("Unable to include Fervor autoupdater.")
#}

#!include(../third-party/tasteful-server/tasteful-server.pri) {
#    error("Unable to include tasteful-server.")
#}

OTHER_FILES += \
    qml/qml/main.qml \
    MailPopup.qml \
    HostIdPopup.qml \
    ComboBox1.qml \
    DropDownMenu.qml \
    MyInfo.plist

#CONFIG += CONSOLE
win32{
    RC_FILE =blink1.rc
}
#ICON = images/blink1-icon01.ico

mac{
    ICON = images/blink1-icon0.icns
    QMAKE_INFO_PLIST = MyInfo.plist
}

