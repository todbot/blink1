#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include "blink1input.h"
#include "blink1pattern.h"
#include "email.h"
#include "hardwaremonitor.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>
#include <QUrlQuery>

class MainWindow;

class HttpServer: public QObject
{
    Q_OBJECT

public:
    HttpServer(QWidget *parent=0);
    void start();
    void stop();
    bool status();
    void setController(MainWindow *mw);
public slots:
    void acceptConnection();
    void startRead();
    void discardClient();
private:
    MainWindow *mw;
    QTcpServer server;
    QList<QTcpSocket *> clientConnections;
};

#endif // HTTPSERVER_H
