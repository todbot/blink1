#ifndef DATAINPUT_H
#define DATAINPUT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileInfo>
#include <QDateTime>
#include <QProcess>
#include <QStringList>
#include <QThread>
#include "blink1input.h"
#include "blink1pattern.h"
#include <QTcpSocket>
class DataInput : public QObject
{
    Q_OBJECT
public:
    DataInput(QObject *parent);
    ~DataInput();

    void start();

    /**
     * Set type of input monitor
     * @param t the type, can be "ifttt", "url", "file", "script"
     */
    void setType(QString t) { type = t; }
    void setRule(QString r) { rule = r; }
    void setInput(Blink1Input* in) { input = in; }
    void setIftttKey(QString iKey) { iftttKey = iKey; }
    QString readColorPattern( QString str);
    QColor readColorCode(QString str);
    bool parsePatternOrColor(QString str, QString type, int lastModTime);

private:
    QNetworkAccessManager *networkManager;
    QNetworkReply *reply;
    QProcess *process;

    QString type;
    QString rule;
public:
    Blink1Input *input;
private:
    QString iftttKey;
    QString processOutput;

    bool readingProcess;
private slots:
    void onFinished();
    void onError();
    void onProcessOutput();
    void onProcessFinished();

signals:
    void toDelete(DataInput*);
    void runPattern(QString, bool);
    void setColor(QColor);
    void iftttToCheck(QString);
    void addReceiveEvent(int date, QString name, QString from);

};

#endif // DATAINPUT_H
