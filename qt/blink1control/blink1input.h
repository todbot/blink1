#ifndef BLINK1INPUT_H
#define BLINK1INPUT_H

#include <QObject>
#include <QJsonObject>
#include "blink1pattern.h"
#include <QDateTime>
#include <QJsonArray>
class Blink1Input : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY updateValues)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY updateValues)
    Q_PROPERTY(QString arg1 READ arg1 WRITE setArg1 NOTIFY updateValues)
    Q_PROPERTY(QString arg2 READ arg2 WRITE setArg2 NOTIFY updateValues)
    Q_PROPERTY(int freq READ freq WRITE setFreq NOTIFY updateValues)
    Q_PROPERTY(int freqCounter READ freqCounter WRITE setFreqCounter NOTIFY updateValues)
    Q_PROPERTY(QString patternName READ patternName WRITE setPatternName NOTIFY updateValues)
    Q_PROPERTY(QString time READ getTime NOTIFY updateTimeValue)

public:
    explicit Blink1Input(QObject *parent = 0);
    
    QJsonObject toJson();
    void fromJson(QJsonObject obj);

    QString name() const;
    void setName(const QString& name);
    QString type() const;
    void setType(const QString& type);
    QString arg1() const;
    void setArg1(const QString& arg1);
    QString arg2() const;
    void setArg2(const QString& arg2);
    QString patternName() const;
    void setPatternName(const QString &patternName);
    void setPause(bool v);
    bool pause();
    qint64 date();
    void setDate(qint64 date);
    QString getTime();
    void setFreq(int freq);
    int freq();
    void setFreqCounter(int freqC);
    int freqCounter();
    void changeFreqCounter();
    void updateTime();
signals:
    void updateValues();
    void updateTimeValue();
public slots:

private:
    QString mname;
    QString mtype;
    QString marg1;
    QString marg2;
    QString mpatternName;
    bool mpause;
    qint64 mdate;
    int mfreq;
    int mfreqCounter;
public:
    bool toDelete;
    bool isChecking;

};

#endif // BLINK1INPUT_H
