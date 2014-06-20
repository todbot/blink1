#-------------------------------------------------
#
#-------------------------------------------------

QT       += core gui widgets network quick qml 
#QT       += core gui widgets network quick qml webkit webkitwidgets
#QT       += core gui widgets network

#CONFIG += debug
#CONFIG += release
#CONFIG += console
#CONFIG -= app_bundle

TARGET = Blink1Control
TEMPLATE = app
VERSION = v1.83
#VERSION = $$system(git describe)  # should return tag like "v1.8"
# but git isn't in PATH in Windows, so can't do it.

qmlfolder.source = qml/qml
qmlfolder.target = qml
helpfolder.source = help
helpfolder.target = help
DEPLOYMENTFOLDERS = qmlfolder helpfolder

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


SOURCES += main.cpp\
        mainwindow.cpp \
    blink1pattern.cpp \
    blink1input.cpp \
    datainput.cpp \
    bigbuttons.cpp \
    email.cpp \
    simplecrypt.cpp \
    hardwaremonitor.cpp \
    Cursorshapearea.cpp \
    httpserver.cpp

HEADERS  += mainwindow.h \
    blink1pattern.h \
    blink1input.h \
    datainput.h \
    bigbuttons.h \
    email.h \
    simplecrypt.h \
    hardwaremonitor.h \
    Cursorshapearea.h \
    patternsReadOnly.h \
    httpserver.h

#FORMS    += mainwindow.ui

RESOURCES += \
    myresources.qrc


# for blink1-lib
# note: need to "cd blink1/commandline && make lib" beforehand
BLINK1_LIB_DIR=$$PWD/../../commandline
#message("BLINK1_LIB_DIR=$$BLINK1_LIB_DIR")

macx:  LIBS += -L$$BLINK1_LIB_DIR -lBlink1
win32: LIBS += $$BLINK1_LIB_DIR/blink1-lib.dll

DEFINES += BLINK1CONTROL_VERSION=\\\"$$VERSION\\\"
#message("DEFINES = $$DEFINES")

QMAKE_CXXFLAGS += -DUSE_HIDAPI 
#QMAKE_CFLAGS += -DUSE_HIDAPI
INCLUDEPATH += $$BLINK1_LIB_DIR
DEPENDPATH  += $$BLINK1_LIB_DIR


macx {
    MYAPPDIR=$$OUT_PWD/$${TARGET}.app/Contents/MacOS
    #message( "MYAPPDIR = $$MYAPPDIR" )
    BLINK1LIBPATH = $$BLINK1_LIB_DIR/libBlink1.dylib
    QMAKE_POST_LINK += $(COPY) $$BLINK1LIBPATH $$MYAPPDIR
}

win32 {
    CONFIG(release, debug|release):  MYAPPDIR=$$OUT_PWD/release
    CONFIG(debug,   debug|release):  MYAPPDIR=$$OUT_PWD/debug
    #message( "MYAPPDIR = $$MYAPPDIR , DESTDIR = $(DESTDIR), helpfolder = $$helpfolder" )
    BLINK1LIBPATH = $$BLINK1_LIB_DIR/blink1-lib.dll
    BLINK1LIBPATH ~= s,/,\\,g   # Windows-ify the path
    MYAPPDIR ~= s,/,\\,g   # Windows-ify the path
    QMAKE_POST_LINK += $(COPY) $$BLINK1LIBPATH $$MYAPPDIR
    # the below doesn't work
    #QMAKE_POST_LINK += & $(MKDIR) help\help
    #QMAKE_POST_LINK += & $(COPY_DIR) help\help $$MYAPPDIR
}


OTHER_FILES += \
    qml/qml/main.qml \
    MailPopup.qml \
    HostIdPopup.qml \
    ComboBox1.qml \
    DropDownMenu.qml \
    MyInfo.plist \
    MyGroupBox.qml \
    help/index.html

win32{
    RC_FILE =blink1.rc
}
#ICON = images/blink1-icon01.ico

mac{
    ICON = images/blink1-icon0.icns
    QMAKE_INFO_PLIST = MyInfo.plist
}



# win32:
# minimum DLLs appear to be: (in "release" kit)
# Qt5Core.dll Qt5Gui.dll Qt5Widgets.dll
# icudt49.dll icuin49.dll icuuc49.dll
# D3DCompiler_43.dll libGLESv2.dll libEGL.dll
# libgcc_s_sjlj-1.dll libstdc++-6.dll  libwinpthread-1.dll
# blink1-lib.dll


# for tasteful-server
##INCLUDEPATH += ../third-party/tasteful-server/include
##macx: LIBS += -L../third-party/tasteful-server -lTastefulServer

# Fervor autoupdater
#!include("../third-party/fervor/Fervor.pri") {
#    error("Unable to include Fervor autoupdater.")
#}

#!include(../third-party/tasteful-server/tasteful-server.pri) {
#    error("Unable to include tasteful-server.")
#}
