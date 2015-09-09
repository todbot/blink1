#include "httpserver.h"
#include "mainwindow.h"

#include <QSettings>

HttpServer::HttpServer(QWidget *parent) :
    QObject(parent)
{
    host = "localhost";
    port = 8934;
}
void HttpServer::setController(MainWindow *mw){
    this->mw=mw;
}

void HttpServer::setHost(QString h) {
    this->host = h;
}
void HttpServer::setPort( int p ) {
    this->port = p;
}

void HttpServer::start() {

    QHostAddress haddr = QHostAddress::LocalHost;  // default
    if( host == "localhost" ) { // do nothing this is the default
    } else if( host == "any" ) {
        haddr = QHostAddress::AnyIPv4;
    } else if( host != "" ) { 
        haddr = QHostAddress( host );
    }
    connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
    server.listen( haddr, port );
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

        client->write("HTTP/1.0 200 OK\r\n");  // FIXME: not always 200 OK
        client->write("Connecton: close\r\n");
        client->write("Content-type: text/plain\r\n");
        client->write("Access-Control-Allow-Origin: *\r\n");
        client->write("\r\n");

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
            QJsonArray ja=mw->getCachedBlinkId();
            resp.insert("blink1_serialnums",ja);
            resp.insert("blink1control_version", QString(BLINK1CONTROL_VERSION) );
            resp.insert("blink1control_config", settings.fileName() );
            resp.insert("logging", mw->getLogging());
            resp.insert("logfile", mw->getLogFileName());
            resp.insert("status",QString("blink1 id"));
        }
        else if( cmd == "/regenerateblink1id" ) {
            QString oldIftttKey=mw->getIftttKey();
            resp.insert("blink_id_old",oldIftttKey);
            mw->regenerateBlink1Id();
            resp.insert("blink_id",mw->getIftttKey());
            resp.insert("status",QString("regenerate id"));
        }
        /*
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
        */
        else if( cmd == "/fadeToRGB" || cmd == "/on" || cmd == "/off" ) {
            bool ok;
            QString cstr = qurlquery.queryItemValue("rgb");
            double time  = qurlquery.queryItemValue("time").toDouble(&ok);
            QString idstr= qurlquery.queryItemValue("id");
            int ledn     = qurlquery.queryItemValue("ledn").toInt();

            if( ledn < 0 || ledn > blink1_max_devices ) ledn = 0;
            if( !ok ) time = 0.1;  // default time

            QStringList ids;
            if( idstr != "" ) { 
                ids = idstr.split(",", QString::SkipEmptyParts);
            }
            if( ids.size() == 0 ) { // hack so we can iterate over ids list in all cases
                ids.append( mw->getBlinkKey() );  // FIXME: omg marcin, the naming
            }

            QColor c;
            QString status;
            if( cmd == "/off" )     cstr = "#000000";
            else if( cmd == "/on" ) cstr = "#FFFFFF";

            c = QColor(cstr);
            if( c.isValid() ) {           // do main thing
                resp.insert("ids", QJsonArray::fromStringList(ids));
                status = "fadeToRGB: "+cstr+" t:"+QString::number(time);
                for( int i=0; i<ids.size(); i++ ) {
                    QString id = ids.at(i);
                    //blink1_getCacheIndexById( 
                    //if( id == mw->getBlinkKey() ) {  // FIXME: ugh this is so hacky
                    //    mw->stopPattern(mw->getActivePatternName());
                    //    mw->setColorToBlinkN( c,time*1000,ledn);
                    //}
                    //else { 
                        //mw->blink1SetColorById( c, time*1000, id, ledn );
                    emit blink1SetColorById( c, time*1000, id, ledn );
                        //}
                }
            } else { 
                status = "fadeToRGB: invalid color";
            }
            resp.insert("ledn",ledn);
            resp.insert("rgb",cstr);
            resp.insert("time",QString::number(time));
            resp.insert("status", status);
        }
        else if( cmd=="/lastColor" ) {
            resp.insert("lastColor",mw->getCurrentColor().name());
            resp.insert("status",QString("last color"));
        }
        else if( cmd=="/logging" ) {
            QString levelstr="";
            if( qurlquery.hasQueryItem("loglevel") ) {
                levelstr = qurlquery.queryItemValue("loglevel");
            } else if( qurlquery.hasQueryItem("logLevel") ) {
                levelstr = qurlquery.queryItemValue("logLevel");
            }
            if( levelstr == "1" || levelstr == "0" ) {
                mw->startOrStopLogging(levelstr.toInt());
            }

            resp.insert("loglevel",(mw->getLogging()) ? 1 : 0);
            resp.insert("logfile", mw->getLogFileName());
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
            QString pname;
            if( qurlquery.hasQueryItem("pname") ) {
                pname = qurlquery.queryItemValue("pname");
            }
            QJsonArray qarrp;
            QMap<QString,Blink1Pattern *>patterns=mw->getFullPatternList();
            foreach (QString nm, patterns.keys() ) {
                if( pname == "" || pname == nm ) {  // only add if all or a specific one
                    QJsonObject obj = patterns.value(nm)->toJson(); //WithNameAndPatternStr();
                    qarrp.append(obj);
                }
            }
            resp.insert("patterns",qarrp);
            resp.insert("status",QString("patterns"));
        }
        else if( cmd=="/pattern/play" ) {
            QString pname = qurlquery.queryItemValue("pname");
            pname.replace('+', ' ');
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
            pname.replace('+', ' ');
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
            pname.replace('+', ' ');
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
            pname.replace('+', ' ');
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
