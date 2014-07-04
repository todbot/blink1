#include <QCoreApplication>
#include <QDebug>

#include "mainapp.h"

int main(int argc, char *argv[])
{
    MainApp app(argc, argv);

    qDebug() << "mac-hwardware-tests!";

    app.checkBattery();

    return app.exec();
}
