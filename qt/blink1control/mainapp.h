#ifndef MAINAPP_H
#define MAINAPP_H

#include <QApplication>

class MainApp : public QApplication
{

 public:
    MainApp(int &argc, char **argv);
    ~MainApp() {};
    bool notify(QObject* receiver, QEvent* event);
    

};


#endif
