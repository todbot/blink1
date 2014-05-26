#include "mainwindow.h"
#include <QApplication>
int main(int argc, char *argv[])
{

/*
// potential fix for 
// "Failed to load platform plugin “cocoa”" error during deplay
// http://qt-project.org/forums/viewthread/26446

    if deploy:
        print("DEPLOY")
        for path in QApplication.libraryPaths():
            QApplication.removeLibraryPath(path)

        filePath = os.path.dirname(os.path.abspath( __file__ ))
        # abspath returns "APP_DIR/Contents/Resouces". This isn't same to C++ app.
        fileDir = QDir(filePath)
        fileDir.cdUp()
        fileDir.cd("PlugIns")
        appPath = fileDir.absolutePath()
        QApplication.addLibraryPath(str(appPath))

    print("INIT: " + str(QApplication.libraryPaths()))
    QApplication(sys.argv)

    //todbot */
    QApplication a(argc, argv);
    QApplication::setApplicationName("blink1controlqt");
    QApplication::setApplicationVersion("0.9.9");
    QApplication::setOrganizationName("ThingM");
    QApplication::setOrganizationDomain("thingm.com");

    MainWindow w;
    //w->show();

    return a.exec();
}
