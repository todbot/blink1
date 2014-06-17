#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // is this info used ever?  is it for the plist?
    //QApplication::setApplicationName("blink1control");
    //QApplication::setApplicationVersion( QString(BLINK1CONTROL_VERSION) );
    //QApplication::setOrganizationName("ThingM");
    //QApplication::setOrganizationDomain("thingm.com");

    MainWindow w;  // this seems messed up, why even use mainwindow?
    //w.show();

    return a.exec();
}
