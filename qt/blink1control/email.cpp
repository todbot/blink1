#include "email.h"

Email::Email(QString name,QObject *parent) :
    QObject(parent)
{
    this->name=name;
    simpleCrypt =  new SimpleCrypt(Q_UINT64_C(0xbf2fd4a2fcb9f00f));
    this->type=0;
    this->server="";
    this->login="";
    this->passwd="";
    this->port=0;
    this->ssl=false;
    this->result=0;
    this->parser="";
    this->patternName="";
    this->lastid=-1;
    this->freq=60;
    this->freqCounter=1;
    this->value="NO VALUE";
    this->email="";
    this->unreadCount=0;
    markAsUnread=false;
    socket=NULL;
    action_number=0;
    edit=false;
    socket=NULL;
    matchingMessagesCount=0;
    tmpLastId=-1;
    recentEventAdded=false;
    time=new QTimer();
    connect(time,SIGNAL(timeout()),SLOT(checkState()));
    this->editing=false;
}
Email::~Email(){
    if(socket!=NULL){
        socket->abort();
        delete socket;
    }
    if(time!=NULL){
        disconnect(time);
        time->stop();
        delete time;
    }
    if(simpleCrypt!=NULL)
        delete simpleCrypt;
}

void Email::checkMail(){
    qDebug()<<"checking mail "<<name<<" ...";
    addToLog("checking mail "+name+" ...");
    if(editing){
        qDebug()<<name<<" in edit mode!";
        addToLog(name+" in edit mode!");
        return;
    }
    if(edit){
        if(socket!=NULL){
            if(!socket->isOpen()){
                delete socket;
                socket=NULL;
                edit=false;
            }
        }else{
            edit=false;
        }
    }
    if(socket==NULL){
        if(!ssl)
            socket=new QTcpSocket(this);
        else
            socket=new QSslSocket(this);

        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(sockerErrorSlot(QAbstractSocket::SocketError)));

        if(type==0 || type==2)
            connect(socket,SIGNAL(readyRead()),this,SLOT(readyIMAP()));
        else if(type==1)
            connect(socket,SIGNAL(readyRead()),this,SLOT(readyPOP3()));
        if(ssl){
            connect(((QSslSocket*)socket),SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(sslErrors(QList<QSslError>)));
        }
    }

    if(!socket->isOpen()){
        errorsList.clear();
        emit errorsUpdate();
        matchingMessagesCount=0;
        tmpLastId=-1;
        recentEventAdded=false;
        setValue("Checking...");
        downloadedMessage="";
        if(ssl)
            ((QSslSocket*)socket)->connectToHostEncrypted(server,port);
        else
            socket->connectToHost(server,port);
        action_number=0;
        markAsUnread=false;
        //time->singleShot(5000,this,SLOT(checkState()));
        time->setSingleShot(true);
        time->start(5000);
    }
}
void Email::checkState(){
    qDebug()<<"~~~~STATE~~~~";
    qDebug()<<socket->state();
    if(socket->state()==2){
        socket->close();
        value="CONNECTION ERROR";
        errorsList.append("Connection timeout");
        emit errorsUpdate();
        emit updateOnlyTextStatus();
    }
}

void Email::readyIMAP(){
    QString tmp=socket->readAll().data();
    downloadedMessage+=tmp;
    qDebug()<<"readyIMAP: ******* downloadedMessage:\n"<< downloadedMessage;
    addToLog("DOWNLOADED: "+downloadedMessage);
    if(downloadedMessage.indexOf("a00"+QString::number(action_number)+" OK")==-1 && downloadedMessage.indexOf("a00"+QString::number(action_number)+" NO")==-1 && downloadedMessage.indexOf("a00"+QString::number(action_number)+" BAD")==-1 && action_number!=0)
        return;
    qDebug()<<"DOWNLOADED WHOLE MESSAGE!";
    addToLog("DOWNLOADED WHOLE MESSAGE!");
    if(action_number==1){
        if(downloadedMessage.indexOf("NO [AUTHENTICATIONFAILED]")!=-1){
            value="WRONG LOGIN OR PASSWORD";
            emit updateOnlyTextStatus();
            socket->close();
            return;
        }
        if(downloadedMessage.indexOf("a001 NO Invalid")!=-1){
            value="WRONG LOGIN OR PASSWORD";
            emit updateOnlyTextStatus();
            socket->close();
            return;
        }
        if(downloadedMessage.indexOf("a001 NO Login")!=-1){
            value="WRONG LOGIN OR PASSWORD";
            emit updateOnlyTextStatus();
            socket->close();
            return;
        }
    }
    if(downloadedMessage.indexOf("a00"+QString::number(action_number)+" NO")!=-1){
        value="CONNECTION ERROR";
        errorsList.append("Unknown error");
        emit errorsUpdate();
        emit updateOnlyTextStatus();
        socket->close();
        return;
    }
    if(downloadedMessage.indexOf("a00"+QString::number(action_number)+" BAD")!=-1){
        value="CONNECTION ERROR";
        errorsList.append("Internal imap error");
        emit errorsUpdate();
        emit updateOnlyTextStatus();
        socket->close();
        return;
    }
    if(action_number==3 && !markAsUnread){
        if(downloadedMessage.indexOf("* SEARCH")!=-1){
            unreadCount=0;
            QStringList list=downloadedMessage.split(QRegExp("(\\ |\\\n)"));
            qDebug()<<"readyIMAP: list:" << list;
            bool ok=false;
            int pom;
            listOfUnreadEmailsIdToCheckLastId.clear();
            for(int i=2;i<list.count();i++){
                pom=list.at(i).toInt(&ok);
                qDebug()<<pom;
                addToLog(QString::number(pom));
                if(ok && pom!=0) { unreadCount++; unreadEmailsIdList.append(pom); listOfUnreadEmailsIdToCheckLastId.append(pom);}else break;
                ok=false;
            }
            qDebug()<<"LAST ID: "<<lastid;
            addToLog("LAST ID: "+QString::number(lastid));
        }
    }
    if(action_number==4 && !markAsUnread){
        QStringList t=parseMail(downloadedMessage);
        qDebug()<<t;
        markAsUnread=true;
        action_number--;
        QByteArray ba=QString("a003 STORE "+QString::number(tmpLastId)+" -FLAGS (\\Seen)\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"uncheck!";
        addToLog("uncheck");
        downloadedMessage="";
        return;
    }
    if(action_number==3 && markAsUnread){
        qDebug()<<"unchecked!";
        addToLog("unchecked!");
        markAsUnread=false;
    }


    if(action_number==0){
        QByteArray ba=QString("a001 LOGIN "+email+" "+passwd+"\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"sending "<<ba.toLower();
        addToLog("sending "+ba.toLower());
    }else if(action_number==1){
        QByteArray ba=QString("a002 select inbox\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"sending "<<ba.toLower();
        addToLog("sending "+ba.toLower());
    }else if(action_number==2){
        QByteArray ba=QString("a003 search unseen\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"sending "<<ba.toLower();
        addToLog("sending "+ba.toLower());
    }else if(action_number==3){
        if(unreadCount!=0 && unreadEmailsIdList.count()>0){
            QByteArray ba=QString("a004 fetch "+QString::number(unreadEmailsIdList.at(0))+" BODY.PEEK[HEADER.FIELDS (From Subject)]\r\n").toUtf8();
            socket->write(ba);
            qDebug()<<"sending "<<ba.toLower();
            addToLog("sending "+ba.toLower());
            tmpLastId=unreadEmailsIdList.at(0);
            unreadEmailsIdList.removeAt(0);
        }else{
            QByteArray ba=QString("a005 logout\r\n").toUtf8();
            socket->write(ba);
            action_number++;
        }
    }else if(action_number==4){
        QByteArray ba=QString("a005 logout\r\n").toUtf8();
        socket->write(ba);
    }else if(action_number==5){
        // apparently this is where parsing happen
        if(unreadCount>0){
            if(lastid<listOfUnreadEmailsIdToCheckLastId.at(listOfUnreadEmailsIdToCheckLastId.count()-1)){
                if(result==0){  // result==0 means "unread msg count"
                    if(unreadCount>=parser.toInt()){
                        value="New Messages("+QString::number(unreadCount)+")";
                        emit updateOnlyTextStatus();
                        if(!recentEventAdded){
                            emit addReceiveEvent(0,"New Message("+QString::number(unreadCount)+")","Mail:"+name);
                            emit runPattern(patternName,false);
                            recentEventAdded=true;
                        }
                    }else{
                        value="Messages("+QString::number(unreadCount)+")";
                        emit updateOnlyTextStatus();
                    }
                }else if(result==1 || result==2){  // result==1 means "Subject contains", result==2 means "Sender contains"
                    if(matchingMessagesCount>0){
                        value="New Matching Messages("+QString::number(matchingMessagesCount)+")";
                        emit updateOnlyTextStatus();
                        if(!recentEventAdded){
                            emit addReceiveEvent(0,"New messages("+QString::number(matchingMessagesCount)+")",
                                                 "Mail:"+name);
                            emit runPattern(patternName,false);
                            recentEventAdded=true;
                        }
                    }else{
                        value="No Matching Messages";
                        emit updateOnlyTextStatus();
                    }
                }
                lastid=listOfUnreadEmailsIdToCheckLastId.at(listOfUnreadEmailsIdToCheckLastId.count()-1);
            }else{
                if(result==0){
                    value="Messages("+QString::number(unreadCount)+")";
                    emit updateOnlyTextStatus();
                }else if(result==1 || result==2){
                    if(matchingMessagesCount>0){
                        value="Matching Messages("+QString::number(matchingMessagesCount)+")";
                        emit updateOnlyTextStatus();
                    }else{
                        value="No Matching Messages";
                        emit updateOnlyTextStatus();
                    }
                }
                lastid=listOfUnreadEmailsIdToCheckLastId.at(listOfUnreadEmailsIdToCheckLastId.count()-1);
            }
        }else{
            lastid=-1;
            if(result==0){
                value="No New Messages";
                emit updateOnlyTextStatus();
            }else if(result==1 || result==2){
                value="NO MATCHING MESSAGES";
                emit updateOnlyTextStatus();
            }
        }
        socket->close();
        action_number=-100;
    }

    action_number++;
    downloadedMessage="";
}
void Email::readyPOP3(){
    QString tmp=socket->readAll().data();
    qDebug()<<tmp;
    addToLog(tmp);
    if(tmp.startsWith("-ERR Invalid")){
        value="Wrong Login or Password";
        emit updateOnlyTextStatus();
        socket->close();
        return;
    }else if(tmp.startsWith("-ERR Authentication")){
        value="Authentication Error";
        emit updateOnlyTextStatus();
        socket->close();
        return;
    }
    if(action_number==0){
        QByteArray ba=QString("USER "+login+"\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"sending "<<ba.toLower();
        addToLog("sending "+ba.toLower());
    }else if(action_number==1){
        QByteArray ba=QString("PASS "+passwd+"\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"sending "<<ba.toLower();
        addToLog("sending "+ba.toLower());
    }else if(action_number==2){
        QByteArray ba=QString("STAT\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"sending "<<ba.toLower();
        addToLog("sending "+ba.toLower());
    }else if(action_number==3){
        QStringList list=tmp.split(QRegExp("(\\ |\\\n)"));
        if(tmpLastId==-1) tmpLastId=lastid;
        if(list.at(1).toDouble()>lastid && lastid!=-1){
            lastid=list.at(1).toDouble();
            QByteArray ba=QString("RETR "+QString::number(lastid)+"\r\n").toUtf8();
            socket->write(ba);
            qDebug()<<"sending "<<ba.toLower();
            addToLog("sending "+ba.toLower());
        }else{
            lastid=list.at(1).toDouble();
            QByteArray ba=QString("QUIT\r\n").toUtf8();
            socket->write(ba);
            qDebug()<<"sending "<<ba.toLower();
            addToLog("sending "+ba.toLower());
            action_number+=2;
            value="No New Messages";
            emit updateOnlyTextStatus();
        }
    }else if(action_number==5){
        qDebug()<<parseMail(tmp);
        QStringList t=parseMail(tmp);
        if(result==0){
            value="New Message";
            emit updateOnlyTextStatus();
            if(!recentEventAdded){
                emit addReceiveEvent(0,"New Message","Mail:"+name);
                emit runPattern(patternName,false);
                recentEventAdded=true;
            }
        }else if(result==1 || result==2){
            if(matchingMessagesCount>0){
                value="New Message";
                emit updateOnlyTextStatus();
                if(!recentEventAdded){
                    emit addReceiveEvent(0,"New Message","Mail:"+name);
                    emit runPattern(patternName,false);
                    recentEventAdded=true;
                }
            }else{
                value="No Matching Message";
                emit updateOnlyTextStatus();
            }
        }

        QByteArray ba=QString("QUIT\r\n").toUtf8();
        socket->write(ba);
        qDebug()<<"sending "<<ba.toLower();
        addToLog("sending "+ba.toLower());
    }else if(action_number==6){
        socket->close();
    }
    action_number++;
}
void Email::sockerErrorSlot(QAbstractSocket::SocketError e){
    value="Connection Error";
    emit updateOnlyTextStatus();
    socket->close();
    qDebug()<<"ERROR"<<e;
    switch (e) {
    case QAbstractSocket::ConnectionRefusedError:
        errorsList.append("The connection was refused by the peer (or timed out).");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorsList.append("The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.");
        break;
    case QAbstractSocket::HostNotFoundError:
        errorsList.append("The host address was not found.");
        break;
    case QAbstractSocket::SocketAccessError:
        errorsList.append("The socket operation failed because the application lacked the required privileges.");
        break;
    case QAbstractSocket::SocketResourceError:
        errorsList.append("The local system ran out of resources (e.g., too many sockets).");
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorsList.append("The socket operation timed out.");
        break;
    case QAbstractSocket::DatagramTooLargeError:
        errorsList.append("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).");
        break;
    case QAbstractSocket::NetworkError:
        errorsList.append("An error occurred with the network (e.g., the network cable was accidentally plugged out).");
        break;
    case QAbstractSocket::AddressInUseError:
        errorsList.append("The address specified to QUdpSocket::bind() is already in use and was set to be exclusive.");
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        errorsList.append("The address specified to QUdpSocket::bind() does not belong to the host.");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        errorsList.append("The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).");
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        errorsList.append("The socket is using a proxy, and the proxy requires authentication.");
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        errorsList.append("The SSL/TLS handshake failed, so the connection was closed.");
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        errorsList.append("The last operation attempted has not finished yet (still in progress in the background).");
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        errorsList.append("Could not contact the proxy server because the connection to that server was denied.");
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        errorsList.append("The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established).");
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        errorsList.append("The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.");
        break;
    case QAbstractSocket::ProxyNotFoundError:
        errorsList.append("The proxy address was not found.");
        break;
    case QAbstractSocket::ProxyProtocolError:
        errorsList.append("The connection negotiation with the proxy server because the response from the proxy server could not be understood.");
        break;
    default:
        errorsList.append("An unidentified error occurred.");
    }
    emit errorsUpdate();
    //errorsList.append(QString(e));
}

// what in the hell is this message supposed to do?
// why does it do it like this?
QStringList Email::parseMail(QString mail){
    int sub=mail.indexOf("Subject:");
    int from=mail.indexOf("From:");
    QStringList lista;
    if(sub==-1 || from==-1) return lista;
    lista.append("Subject");
    lista.append(mail.mid(sub+9,mail.indexOf("\n",sub+9)-sub-10));
    lista.append("From");
    lista.append(mail.mid(from+6,mail.indexOf("\n",from+6)-from-7));

    if(result==1){
        if(parser!="" && lista.at(1).indexOf(parser)!=-1){
            matchingMessagesCount++;
        }
    }else if(result==2){
        if(parser!="" && lista.at(3).indexOf(parser)!=-1){
            matchingMessagesCount++;
        }
    }
    qDebug() << "parseMail: "<< lista << "mail: ---\n"<< mail <<"---";
    return lista;
}

void Email::sslErrors(QList<QSslError> l){
    ((QSslSocket*)socket)->ignoreSslErrors();
    qDebug()<<l;
    for(int i=0;i<l.count();i++){
        errorsList.append(l.at(i).errorString());
    }
    emit errorsUpdate();
}
QJsonObject Email::toJson()
{
    QJsonObject obj;
    obj.insert("name", name);
    obj.insert("type",type);
    obj.insert("server",server);
    obj.insert("login",login);
    obj.insert("passwd",simpleCrypt->encryptToString(passwd));
    obj.insert("port",port);
    obj.insert("ssl",ssl);
    obj.insert("result",result);
    obj.insert("parser",parser);
    obj.insert("patternName",patternName);
    obj.insert("lastid",lastid);
    obj.insert("freq",freq);
    obj.insert("freqCounter",0);
    obj.insert("value",value);
    obj.insert("email",email);
    obj.insert("unreadCount",unreadCount);
    return obj;
}
void Email::fromJson( QJsonObject obj)
{
    setName(obj.value("name").toString());
    setType(obj.value("type").toDouble());
    setServer(obj.value("server").toString());
    setLogin(obj.value("login").toString());
    setPasswd(simpleCrypt->decryptToString(obj.value("passwd").toString()));
    setPort(obj.value("port").toDouble());
    setSsl(obj.value("ssl").toBool());
    setResult(obj.value("result").toDouble());
    setParser(obj.value("parser").toString());
    setPatternName(obj.value("patternName").toString());
    setLastid(obj.value("lastid").toDouble());
    setFreq(obj.value("freq").toDouble());
    setFreqCounter(obj.value("freqCounter").toDouble());
    setValue(obj.value("value").toString());
    setEmail(obj.value("email").toString());
    setUnreadCount(obj.value("unreadCount").toDouble());
}
QString Email::getName(){
    return name;
}
void Email::setName(QString name){
    this->name=name;
    emit updateValues();
}
int Email::getType(){
    return type;
}
void Email::setType(int type){
    if(this->type!=type)
        lastid=-1;
    this->type=type;
}
QString Email::getServer(){
    return server;
}
void Email::setServer(QString server){
    this->server=server;
    emit updateValues();
}
QString Email::getLogin(){
    return login;
}
void Email::setLogin(QString login){
    this->login=login;
    emit updateValues();
}
QString Email::getPasswd(){
    return passwd;
}
void Email::setPasswd(QString passwd){
    this->passwd=passwd;
}
int Email::getPort(){
    return port;
}
void Email::setPort(int port){
    this->port=port;
}
int Email::getResult(){
    return result;
}
void Email::setResult(int result){
    this->result=result;
}
bool Email::getSsl(){
    return ssl;
}
void Email::setSsl(bool ssl){
    this->ssl=ssl;
}
QString Email::getParser(){
    return parser;
}
void Email::setParser(QString parser){
    this->parser=parser;
}
QString Email::getPatternName(){
    return patternName;
}
void Email::setPatternName(QString patternName){
    this->patternName=patternName;
    emit updateValues();
}
int Email::getLastid(){
    return lastid;
}
void Email::setLastid(int lastid){
    this->lastid=lastid;
}
int Email::getFreq(){
    return freq;
}
void Email::setFreq(int freq){
    this->freq=freq;
    emit updateValues();
}
int Email::getFreqCounter(){
    return freqCounter;
}
void Email::setFreqCounter(int freqCounter){
    this->freqCounter=freqCounter;
    emit updateValues();
}
QString Email::getValue(){
    return value;
}
void Email::setValue(QString value){
    this->value=value;
    emit updateOnlyTextStatus();
}
QString Email::getEmail(){
    return email;
}
void Email::setEmail(QString email){
    this->email=email;
    emit updateValues();
}
int Email::getUnreadCount(){
    return unreadCount;
}
void Email::setUnreadCount(int unreadCount){
    this->unreadCount=unreadCount;
    emit updateValues();
}
void Email::changeFreqCounter(){
    if(freq==0) freq=3;
    freqCounter=(freqCounter+1)%freq;
}
void Email::setEdit(bool edit){
    this->edit=edit;
}
QVariantList Email::getErrorsList(){
    QVariantList eList;
    for(int i=0;i<errorsList.count();i++){
        eList.append(QVariant(errorsList.at(i)));
    }
    return eList;
}
void Email::setEditing(bool e){
    this->editing=e;
    this->value="In edit mode";
    if(this->socket!=NULL)
        this->socket->close();
    emit updateOnlyTextStatus();
}
