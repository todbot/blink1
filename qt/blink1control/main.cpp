#include "mainapp.h"
#include "mainwindow.h"
#include <QApplication>
#include <QSharedMemory>

QSharedMemory sharedMemory;


int main(int argc, char *argv[])
{
    MainApp a(argc,argv);

    // had to disable this for now on Mac,
    // weird crash deep in Qt leaves shared memory segment dangling
#ifdef Q_OS_WIN
    // http://qt-project.org/doc/qt-5/qtcore-sharedmemory-example.html
    // http://qt-project.org/forums/viewthread/18262/#89444
    sharedMemory.setKey("Blink1ControlShMemKey");
    if( !sharedMemory.create(1) ) {
        qDebug() << "Blink1Control already running";
        QMessageBox::about(0, QString("Blink1Control running"), 
                           "Blink1Control is already running.");
#ifdef Q_OS_WIN
        a.processEvents();  // FIXME: why are different OSes different here?
        return -1;
#else
        return a.exec();
#endif
    }

#endif
   
    MainWindow w;  // this seems messed up, why even use mainwindow?

    return a.exec();
}
