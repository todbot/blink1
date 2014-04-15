#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("blink1controlqt");
    QApplication::setApplicationVersion("0.9.9");
    QApplication::setOrganizationName("ThingM");
    QApplication::setOrganizationDomain("thingm.com");

    MainWindow *w=new MainWindow();
    //w->show();

    return a.exec();
}
