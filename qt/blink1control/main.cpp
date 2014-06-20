#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;  // this seems messed up, why even use mainwindow?
    //w.show();

    return a.exec();
}
