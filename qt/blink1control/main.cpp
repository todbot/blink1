#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // is this info used ever?  is it for the plist?
    QApplication::setApplicationName("blink1control");
    QApplication::setApplicationVersion("0.9.9");
    QApplication::setOrganizationName("ThingM");
    QApplication::setOrganizationDomain("thingm.com");

    MainWindow w;
    //w->show();

    return a.exec();
}
