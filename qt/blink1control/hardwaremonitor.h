#ifndef HARDWAREMONITOR_H
#define HARDWAREMONITOR_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QApplication>
#include <QProcess>
#include <QStringList>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

class HardwareMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY updateValues)
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY updateValues)
    Q_PROPERTY(int lvl READ getLvl WRITE setLvl NOTIFY updateValues)
    Q_PROPERTY(int action READ getAction WRITE setAction NOTIFY updateValues)
    Q_PROPERTY(QString patternName READ getPatternName WRITE setPatternName NOTIFY updateValues)
    Q_PROPERTY(QString status READ getStatus WRITE setStatus NOTIFY updateOnlyStatusAndCurrentValue)
    Q_PROPERTY(int freq READ getFreq WRITE setFreq NOTIFY updateValues)
    Q_PROPERTY(int value READ getValue WRITE setValue NOTIFY updateOnlyStatusAndCurrentValue)
    Q_PROPERTY(int extraValue READ getExtraValue WRITE setExtraValue NOTIFY updateValues)
    Q_PROPERTY(bool done READ getDone WRITE setDone NOTIFY updateOnlyStatusAndCurrentValue)
    Q_PROPERTY(int role READ getRole WRITE setRole NOTIFY updateValues)
public:
    explicit HardwareMonitor(QString name,QObject *parent = 0);
    ~HardwareMonitor();
    QString getName();
    void setName(QString name);
    int getType();
    void setType(int type);
    int getLvl();
    void setLvl(int lvl);
    int getAction();
    void setAction(int action);
    QString getPatternName();
    void setPatternName(QString patternName);
    int getFreq();
    void setFreq(int freq);
    int getValue();
    void setValue(int value);
    int getExtraValue();
    void setExtraValue(int extraValue);
    int getFreqCounter();
    void setFreqCounter(int freqCounter);
    QJsonObject toJson();
    void fromJson( QJsonObject obj);
    void changeFreqCounter();
    QString getStatus();
    bool getDone();
    int getRole();
    void setRole(int role);
    bool checkValue(int value);
    void setStatus(QString status);
    void setDone(bool done);
signals:
    void updateValues();
    void updateOnlyStatusAndCurrentValue();
    void runPattern(QString,bool);
    void addReceiveEvent(int,QString,QString);
    void addToLog(QString txt);
public slots:
    void checkMonitor();
    void checkBattery();
    void checkCpu();
    void checkRam();
    void readyReadCpu(int);
    void readyReadRam(int);
    void readyReadBattery(int);
    void setEditing(bool edit);
private:
    QString name,patternName,status;
    int type;               //0-battery, 1-cpu, 2-ram,
    int action;             //0-< 1-<= 2-= 3-> 4->=
    int role;               //0-activity, 1-alert, 2-both
    int lvl,freq,value,extraValue,freqCounter;
    bool done,editing;
    QProcess *p,*p2,*p3;
};

#endif // HARDWAREMONITOR_H
