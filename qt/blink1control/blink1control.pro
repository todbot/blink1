#-------------------------------------------------
#
#-------------------------------------------------

QT       += core gui widgets network quick qml 

#CONFIG += console  # Win: uncomment to have console window open up for debugging
#CONFIG -= app_bundle # Mac: uncommment to not build a .app bundle
#CONFIG += qtquickcompiler

TARGET = Blink1Control
TEMPLATE = app

VERSION = 1.91
BLINK1CONTROL_VERSION = "v$$VERSION"

#VERSION = $$system(git describe)  
# should return tag like "v1.8"
# but git isn't in PATH in Windows, so can't do it.

# DEPLOYMENT FOLDERS is used by qtquick2applicationviewer.pri, not qmake
qmlfolder.source = qml/qml
qmlfolder.target = qml
helpfolder.source = help
helpfolder.target = help
DEPLOYMENTFOLDERS = qmlfolder helpfolder

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()  # defined by qtquickapplicationviewer


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
    httpserver.cpp \
    mainapp.cpp

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
    httpserver.h \
    mainapp.h

#FORMS    += mainwindow.ui

RESOURCES += \
    myresources.qrc


# for blink1-lib
# note: be sure to compile blink1-lib beforhard with:
# cd blink1/commandline && make lib"
# for Windows, that will build both MinGW & MSVC compatible libs
BLINK1_LIB_DIR=$$PWD/../../commandline
#message("BLINK1_LIB_DIR=$$BLINK1_LIB_DIR")

macx:  LIBS += -L$$BLINK1_LIB_DIR -lBlink1
#win32: LIBS += $$BLINK1_LIB_DIR/blink1-lib.dll
win32-g++: LIBS += $$BLINK1_LIB_DIR/blink1-lib.dll
win32-msvc*: LIBS += $$BLINK1_LIB_DIR/blink1-lib.lib

DEFINES += BLINK1CONTROL_VERSION=\\\"$$BLINK1CONTROL_VERSION\\\"
#message("DEFINES = $$DEFINES")

QMAKE_CXXFLAGS += -DUSE_HIDAPI # not QMAKE_CFLAGS += -DUSE_HIDAPI
INCLUDEPATH += $$BLINK1_LIB_DIR
DEPENDPATH  += $$BLINK1_LIB_DIR

macx {
    MYAPPDIR=$$OUT_PWD/$${TARGET}.app/Contents/MacOS
    BLINK1LIBPATH = $$BLINK1_LIB_DIR/libBlink1.dylib
    QMAKE_POST_LINK += $(COPY) $$BLINK1LIBPATH $$MYAPPDIR
    #message( "MYAPPDIR = $$MYAPPDIR" )
}

win32 {
    CONFIG(release, debug|release):  MYAPPDIR=$$OUT_PWD/release
    CONFIG(debug,   debug|release):  MYAPPDIR=$$OUT_PWD/debug
    BLINK1LIBPATH = $$clean_path($$BLINK1_LIB_DIR/blink1-lib.dll)
    QMAKE_PRE_LINK  += $(COPY) $$shell_path($$BLINK1LIBPATH) $$shell_path($$MYAPPDIR)
    #message( "MYAPPDIR = $$MYAPPDIR , DESTDIR = $(DESTDIR), helpfolder = $$helpfolder" )
}


OTHER_FILES += \
    README.txt \
    MyInfo.plist \
    help/index.html

win32{
    RC_FILE =blink1.rc
#    CONFIG += embed_manifest_exe
}
#ICON = images/blink1-icon01.ico

macx{
    ICON = images/blink1-icon0.icns
    QMAKE_INFO_PLIST = MyInfo.plist
}

