#include "httpserver.h"
#include "mainwindow.h"

#include <QSettings>

HttpServer::HttpServer(QWidget *parent) :
    QObject(parent)
{
}
void HttpServer::setController(MainWindow *mw){
    this->mw=mw;
}

void HttpServer::start(){
    connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
    server.listen(QHostAddress::LocalHost, 8934);
}
void HttpServer::stop(){
    for(int i=0;i<clientConnections.count();i++)
        clientConnections.at(i)->abort();
    clientConnections.clear();
    server.close();
}
void HttpServer::acceptConnection(){
    while(server.hasPendingConnections()){
      QTcpSocket *client=server.nextPendingConnection();
      connect(client, SIGNAL(readyRead()),this, SLOT(startRead()));
      connect(client, SIGNAL(disconnected()),this,SLOT(discardClient()));
      clientConnections.append(client);
    }
}
void HttpServer::startRead(){
        QTcpSocket *client=(QTcpSocket*)sender();
        QString mssg=client->readLine();

        client->write("HTTP/1.0 200 OK\n");  // FIXME: not always 200 OK
        client->write("Connecton: close\n");
        client->write("Content-type: text/plain\n");
        client->write("\n");

        QJsonObject resp;  // response object

        // msg line in form: "GET /path?querystring HTTP/1.0", we want middle part
        QStringList qsl = mssg.split(" ");
        if( qsl.count() < 2 ) {
            resp.insert("status",QString("bad request"));  // FIXME:should set HTTP header
            QJsonDocument jd(resp);
            client->write( jd.toJson() );
            client->close();
            return;
        }

        // get /path?querystring part, ignore HTTP method & version for now
        QString query = qsl[1];
        QUrl qurl = QUrl( query );
        QUrlQuery qurlquery = QUrlQuery( qurl.query() );
        QString path = qurl.path();

        path = path.replace( QRegExp("/$"), ""); // replace optional trailing slash from path
        QString cmd = QString(path).replace("/blink1","");  // remove /blink1 cmd prefix

        // command parsing
        if( cmd == "/id" ) {
            QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ThingM", "Blink1Control");  // FIXME:
            resp.insert("blink_id",mw->getIftttKey());
            QJsonArray ja=mw->getCatchedBlinkId();
            resp.insert("blink1_serialnums",ja);
            resp.insert("blink1control_version", QString(BLINK1CONTROL_VERSION) );
            resp.insert("blink1control_config", settings.fileName() );
            resp.insert("status",QString("blink1 id"));
        }
        else if( cmd == "/regenerateblink1id" ) {
            QString oldIftttKey=mw->getIftttKey();
            resp.insert("blink_id_old",oldIftttKey);
            mw->regenerateBlink1Id();
            resp.insert("blink_id",mw->getIftttKey());
            resp.insert("status",QString("regenerate id"));
        }
        else if( cmd == "/on" ) {
            mw->stopPattern(mw->getActivePatternName());
            QString cstr = "#FFFFFF";
            double time=0.1;
            resp.insert("rgb",cstr);
            resp.insert("status", cmd+": "+cstr+" t:"+QString::number(time));
            resp.insert("time",QString::number(time));
            mw->setColorToBlink(QColor(cstr),time*1000);
        }
        else if( cmd == "/off" ) {
            mw->stopPattern(mw->getActivePatternName());
            QString cstr = "#000000";
            double time=0.1;
            resp.insert("rgb",cstr);
            resp.insert("status", cmd+": "+cstr+" t:"+QString::number(time));
            resp.insert("time",QString::number(time));
            mw->setColorToBlink(QColor(cstr),time*1000);
        }
        else if( cmd == "/fadeToRGB" ) {
            mw->stopPattern(mw->getActivePatternName());
            bool ok;
            QString cstr = qurlquery.queryItemValue("rgb");
            double time  = qurlquery.queryItemValue("time").toDouble(&ok);
            QColor c = QColor(cstr);
            QString status = "fadeToRGB: invalid color";
            if( !ok ) time = 0.1;
            if( c.isValid() ) {
                status = "fadeToRGB: "+cstr+" t:"+QString::number(time);
                mw->setColorToBlink( c,time*1000);
            }
            resp.insert("rgb",cstr);
            resp.insert("time",QString::number(time));
            resp.insert("status", status);
        }
        else if( cmd=="/lastColor" ) {
            resp.insert("lastColor",mw->getCurrentColor().name());
            resp.insert("status",QString("last color"));
        }
        else if( cmd=="/logging" ) {
            QString levelstr = qurlquery.queryItemValue("loglevel");
            if( levelstr == "1" || levelstr == "0" ) {
                mw->startOrStopLogging(levelstr.toInt());
            }
            resp.insert("loglevel",mw->getLogging());
            resp.insert("status",QString("logging"));
        }
        else if( cmd=="/input" || cmd=="/inputs" ) {
            int enable=-1; // -1 == enable wasn't specified
            QMap<QString,Blink1Input*> inputs=mw->getFullInputList();
            QString enablestr = qurlquery.queryItemValue("enable");
            if( enablestr == "on" || enablestr=="off" ) {  // FIXME: find better way to do this
                enable = (enablestr=="on") ? 1:0;
            }
            QJsonArray qarrp;
            foreach (QString nm, inputs.keys() ) {  /// FIXME: this prints wrong name
                qDebug()<<"httpserver /input: "<<nm;
                QJsonObject obj = inputs.value(nm)->toJson();
                qarrp.append(obj);
                if( enable == 1 || enable == 0 ) {
                    inputs.value(nm)->setPause( (enable)==0 );  // enable or not input 
                }
            }
            resp.insert("enable", (enable!=0||enable!=-1));
            resp.insert("inputs",qarrp);
            resp.insert("status",QString("inputs"));
        }
        else if( cmd=="/input/del" ) {
            //removeInput(tokens[2]);
            //QJsonObject ob;
            //resp.insert("status",QString("input del"));
            resp.insert("status",QString("input/del not implemented yet"));
        }
        else if( cmd=="/input/delall" ) {
            //inputs.clear();
            //emit inputsUpdate();
//            foreach (QString nm, inputs.keys() ) {
//              removeInput(nm);
//              }
//              QJsonObject ob;
//              resp.insert("status",QString("input delall"));
            resp.insert("status",QString("input/dellall not implemented yet"));
        }
        else if( cmd=="/input/ifttt" ) {
            resp.insert("status",QString("input/ifttt not implemented yet"));
        }
        else if( cmd=="/pattern" || cmd=="/patterns" ) {
            QJsonArray qarrp;
            QMap<QString,Blink1Pattern *>patterns=mw->getFullPatternList();
            foreach (QString nm, patterns.keys() ) {
                QJsonObject obj = patterns.value(nm)->toJson(); //WithNameAndPatternStr();
                qarrp.append(obj);
            }
            resp.insert("patterns",qarrp);
            resp.insert("status",QString("patterns"));
        }
        else if( cmd=="/pattern/play" ) {
            QString pname = qurlquery.queryItemValue("pname");
            QString status = "pattern play";
            if( mw->getFullPatternList().contains(pname) ) {
                mw->playPattern(pname);
            } else {
                status = "pattern not found";
            }
            resp.insert("pname", pname);
            resp.insert("status",status);
        }
        else if( cmd=="/pattern/stop" ) {
            QString pname = qurlquery.queryItemValue("pname");
            QString status = "pattern stop";
            if( mw->getFullPatternList().contains(pname) ) {
                mw->stopPattern(pname);
            } else {
                status = "pattern not found";
            }
            resp.insert("pname", pname);
            resp.insert("status",status);
        }
        else if( cmd=="/pattern/add" ) {
            QString pname   = qurlquery.queryItemValue("pname");
            QString pattstr = qurlquery.queryItemValue("pattern");
            QString status = "pattern added";
            if( pname != "" && pattstr != "" ) {
                bool good = mw->addNewPatternFromPatternStr(pname,pattstr);
                if( !good ) {
                    status = "bad pattern";
                }
            } else {
                status = "pattern not added";
            }

            resp.insert("pname", pname);
            resp.insert("pattern", pattstr);  // FIXME: should be parsed pattern
            resp.insert("status", status);
        }
        else if( cmd=="/pattern/del" ) {
            QString pname   = qurlquery.queryItemValue("pname");
            QString status = "pattern deleted";
            if( mw->getFullPatternList().contains(pname) ) {
                mw->stopPattern(pname);
                mw->removePattern(pname);
                mw->updatePatternsList();
            } else {
                status = "pattern not found";
            }
            resp.insert("pname", pname);
            resp.insert("status", status);
        }
        else if( cmd=="/pattern/delall" ) {
            foreach(QString pname,mw->getFullPatternList().keys()){
                mw->stopPattern(pname);
                mw->removePattern(pname);
            }
            mw->updatePatternsList();
            resp.insert("status",QString("pattern delall"));
        }
        else {
            resp.insert("status",QString("unknown command"));
        }

        // write out built-up response to client
        client->write( QJsonDocument(resp).toJson() );
        client->close();
}

void HttpServer::discardClient(){
    QTcpSocket *s=(QTcpSocket*)sender();
    if(!s) return;
    clientConnections.removeAll(s);  // crash here?
    s->deleteLater();
}
bool HttpServer::status(){
    return server.isListening();
}
