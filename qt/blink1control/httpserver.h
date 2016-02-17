#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include "blink1input.h"
#include "blink1pattern.h"
#include "emailcurl.h"
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
    // changing these two requires a stop()/start() to start to new host:port
    void setHost( QString host );
    void setPort( int port );

public slots:
    void acceptConnection();
    void startRead();
    void discardClient();
 
signals:
    void blink1SetColorById( QColor color, int millis, QString blink1serial, int ledn );



private:
    MainWindow *mw;
    QString host;
    int port;
    QTcpServer server;
    QList<QTcpSocket *> clientConnections;
};

#endif // HTTPSERVER_H
