#--*-makefile-*-----------------------------------
# Blink1Control Qt master build file
#
# Windows build requirements:
# - Qt 5.5 w/ 'msvc2013-32-bit' build environment installed
# - Visual Studio 2013  x86 (not 64-bit)
# - MinGW 
#
# Mac OS X build requirements:
# - Qt 5.5
# - Xcode
# - Xcode command-line tools
#
#-------------------------------------------------



QT       += core gui widgets network qml quick

CONFIG += console  # Win: uncomment to have console window open up for debugging
#CONFIG -= app_bundle # Mac: uncommment to not build a .app bundle
#CONFIG += qtquickcompiler

TARGET = Blink1Control
TEMPLATE = app

VERSION = 1.98
BLINK1CONTROL_VERSION = "v$$VERSION"
# note: need to update version in Blink1Control.iss too

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
    simplecrypt.cpp \
    hardwaremonitor.cpp \
    Cursorshapearea.cpp \
    httpserver.cpp \
    mainapp.cpp \
    emailcurl.cpp

HEADERS  += mainwindow.h \
    blink1pattern.h \
    blink1input.h \
    datainput.h \
    bigbuttons.h \
    simplecrypt.h \
    hardwaremonitor.h \
    httpserver.h \
    mainapp.h \
    cursorshapearea.h \
    emailcurl.h

OBJECTIVE_SOURCES += \
    osxFixes.mm

#FORMS    += mainwindow.ui

RESOURCES += \
    myresources.qrc


# blink1-lib begin
# note: be sure to compile blink1-lib beforhand with:
# cd blink1/commandline && make lib"
# for Windows, that will build both MinGW & MSVC compatible libs
BLINK1_LIB_DIR=$$PWD/../../commandline
#message("BLINK1_LIB_DIR=$$BLINK1_LIB_DIR")
macx:        LIBS += -L$$BLINK1_LIB_DIR -lBlink1
win32-g++:   LIBS += $$BLINK1_LIB_DIR/blink1-lib.dll
win32-msvc*: LIBS += $$BLINK1_LIB_DIR/blink1-lib.lib
unix:!macx:  LIBS += $$BLINK1_LIB_DIR/libblink1.so
DEFINES += BLINK1CONTROL_VERSION=\\\"$$BLINK1CONTROL_VERSION\\\"
#message("DEFINES = $$DEFINES")
QMAKE_CXXFLAGS += -DUSE_HIDAPI # not QMAKE_CFLAGS += -DUSE_HIDAPI
INCLUDEPATH += $$BLINK1_LIB_DIR
DEPENDPATH  += $$BLINK1_LIB_DIR

# blink1-lib end

# curl begin
macx {
    DEFINES += CURL_STATICLIB
    LIBS += $$PWD/libcurl-mac/lib/libcurl.a  -lssl -lcrypto -lssl -lcrypto -lz -lldap -lz
    INCLUDEPATH += $$PWD/libcurl-mac/include
}
win32 {
    #LIBS += -L$$PWD/libcurl-win/bin  -lcurl
    #LIBS += -L$$PWD/libcurl-win/bin  -lcurl -lwldap32 -lws2_32
    #DEFINES += CURL_STATICLIB
    LIBS += $$PWD/libcurl-win/lib/libcurldll.a 
    #LIBS += $$PWD/libcurl-win/lib/lib
    INCLUDEPATH += $$PWD/libcurl-win/include

# FIXME: to-do
}
# curl end


# installtion stuff begin

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
    QMAKE_OBJECTIVE_CFLAGS += $${QMAKE_CXXFLAGS}
    LIBS += -framework AppKit -framework Foundation
    APPDIR = $$OUT_PWD/$${TARGET}.app/Contents/MacOS
    #message( "APPDIR = $$APPDIR" )
    #QMAKE_POST_LINK += $(COPY) $$BLINK1LIB $$MYAPPDIR
    #LIBS += -framework Foundation
	BLINK1LIB = $$BLINK1_LIB_DIR/libBlink1.dylib
    copyblink1lib.commands = @echo Copying blink1lib... && $(INSTALL_DIR) \"$$BLINK1LIB\" \"$$APPDIR\"
	QMAKE_EXTRA_TARGETS += copyblink1lib
	first.depends += copyblink1lib
}

unix:!macx {
    BLINK1LIB = $$BLINK1_LIB_DIR/libblink1.so
    copyblink1lib.commands = @echo Copying blink1lib... && $(INSTALL_DIR) \"$$BLINK1LIB\" \"$$OUT_PWD\"
	QMAKE_EXTRA_TARGETS += copyblink1lib
	first.depends += copyblink1lib
}

win32 {
    #QMAKE_CXXFLAGS += /arch:IA32  # turn off avx optimizations
    CONFIG(release, debug|release):  APPDIR=$$OUT_PWD/release
    CONFIG(debug,   debug|release):  APPDIR=$$OUT_PWD/debug
    BLINK1LIB = $$shell_path($$BLINK1_LIB_DIR/blink1-lib.dll)
	CURLLIBS = $$shell_path($$PWD/libcurl-win/bin/*.dll)
	DLLDIR = $$shell_path($$APPDIR)
    copyblink1lib.commands = @echo Copying blink1lib... && $(INSTALL_DIR) \"$$BLINK1LIB\" \"$$DLLDIR\"
    #copyblink1lib.depends = $$DLLDIR/blink1-lib.dll  #(this doesnt' work)
    copycurllibs.commands   = @echo Copying curllibs...   && $(INSTALL_DIR) \"$$CURLLIBS\" \"$$DLLDIR\"
	QMAKE_EXTRA_TARGETS += copyblink1lib copycurllibs
    # enable this for production, too slow during development
	first.depends += copyblink1lib copycurllibs
}

# for XP, see:
# http://stackoverflow.com/questions/11305633/xxxxxx-exe-is-not-a-valid-win32-application#comment20977666_14746749
# http://stackoverflow.com/questions/14657766/visual-studio-2012-platform-toolset-set-incorrectly
# but still doesn't work fully, errors like:
# "strnlen could not be located in msvcrt.dll"
#win32 { 
#  QMAKE_LFLAGS += /SUBSYSTEM:CONSOLE,5.01 
#  QMAKE_CXX += /D_USING_V110_SDK71_ 
#  LIBS *=      -L"%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Lib" 
#  INCLUDEPATH += "%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Include" 
#} 


OTHER_FILES += \
    README.txt \
    MyInfo.plist \
    help/index.html

win32{
    RC_FILE = blink1.rc
#    CONFIG += embed_manifest_exe
}
#ICON = images/blink1-icon01.ico

macx{
    ICON = images/blink1-icon0.icns
    QMAKE_INFO_PLIST = MyInfo.plist
}

