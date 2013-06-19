#include "mainwindow.h"
#include <QApplication>

#include "fvupdater.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("blink1controlqt");
    QApplication::setApplicationVersion("0.9.9");
    QApplication::setOrganizationName("ThingM");
    QApplication::setOrganizationDomain("thingm.com");

    // Set this to your own appcast URL, of course
    FvUpdater::sharedUpdater()->SetFeedURL("http://todbot.com/blink1/tst/blink1control/Appcast.xml");

    // Check for updates silently -- this will not block the initialization of
    // your application, just start a HTTP request and return immediately.
    FvUpdater::sharedUpdater()->CheckForUpdatesSilent();

    MainWindow w;
    w.show();
    
    return a.exec();
}
