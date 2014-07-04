#ifndef MAINAPP_H
#define MAINAPP_H

#include <QCoreApplication>
#include <QProcess>


class MainApp : public QCoreApplication
{
    Q_OBJECT

 public:
    MainApp(int &argc, char **argv);
    ~MainApp() {};
    void checkBattery();
    bool checkValue(int value);

private:   
    QProcess *p3;
    bool done;
    int action;             //0-< 1-<= 2-= 3-> 4->=
    int role;               //0-activity, 1-alert, 2-both
    int lvl;
    int value;
    int extraValue;
    int freqCounter;
    QString status;

public slots:
    void readyReadBattery(int);
    void onError();

};


#endif
