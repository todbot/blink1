
#include "mainapp.h"
#include <typeinfo>

#include <stdio.h>

#include <QMessageBox>

MainApp::MainApp(int &argc, char **argv) : QApplication ( argc, argv ) {};


// from http://stackoverflow.com/questions/4661883/qt-c-error-handling
// but doesn't catch errors like div-by-zero looks like
bool MainApp::notify(QObject* receiver, QEvent* event)
{
    //fprintf(stderr, "notify: got here!\n");
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception &e) {
        fprintf(stderr, "notify: std:exception!\n");
        qFatal("Error %s sending event %s to object %s (%s)", 
                   e.what(), typeid(*event).name(), qPrintable(receiver->objectName()),
                   typeid(*receiver).name());
        QMessageBox::critical(0, "Blink1Control",
                                  "Error " + QString(e.what()) +
                                  " sending event "+ typeid(*event).name() +
                                  " to object "+ receiver->objectName() +
                                  " ("+typeid(*receiver).name()+")");                              
    } catch (...) {
        fprintf(stderr, "notify: other exception!\n");
        qFatal("Error <unknown> sending event %s to object %s (%s)", 
            typeid(*event).name(), qPrintable(receiver->objectName()),
            typeid(*receiver).name());
        QMessageBox::critical(0, "Blink1Control",
                                  "Error <unknown> ");
    }        

    // qFatal aborts, so this isn't really necessary
    // but you might continue if you use a different logging lib
    return false;
}


/*
MainApp::MainApp(int argc, char *argv[]) :
    QApplication(argc,argv)
{

#ifdef Q_OS_MAC
    objc_object* cls = objc_getClass("NSApplication");
    SEL sharedApplication = sel_registerName("sharedApplication");
    objc_object* appInst = objc_msgSend(cls,sharedApplication);

    if(appInst != NULL) {
        objc_object* delegate = objc_msgSend(appInst, sel_registerName("delegate"));
        objc_object* delClass = objc_msgSend(delegate,  sel_registerName("class"));
        const char* tst = class_getName(delClass->isa);
        bool test = class_addMethod((objc_class*)delClass, 
                                    sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:"),
                                    (IMP)dockClickHandler,"B@:");
        if (!test) {
            qDebug() << "failed to register handler...";
        }
    }
#endif

}


#ifdef Q_OS_MAC

#include <objc/objc.h>
#include <objc/message.h>

bool dockClickHandler(id self,SEL _cmd,...)
{
    Q_UNUSED(self)
    Q_UNUSED(_cmd)
   ((MyApplictionClass*)qApp)->onClickOnDock();
     return true;
}

#endif
*/
