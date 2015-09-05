/*
 *
 * Example command-line curl IMAP searching: ('...' is passwd)
 * curl --url imap://mail.example.com -X "EXAMINE INBOX" - Performs a message list on the user's inbox
 * curl -k -u todtest@afklab.com:... --url imaps://mail.afklab.com:993 -X "EXAMINE INBOX"
 * curl -k -u todtest@afklab.com:... --url imaps://mail.afklab.com:993/INBOX -X 'SEARCH NEW'
 * curl -k -u tod.kurt@gmail.com:...  --url imaps://imap.gmail.com:993/INBOX -X 'SEARCH SUBJECT "LinkedIn"'
 * curl -k -u tod.kurt@gmail.com:...  --url 'imaps://imap.gmail.com:993/INBOX/' -X "SEARCH UNSEEN"
 * curl -g -k -u tod.kurt@gmail.com:... --url 'imaps://imap.gmail.com:993/%5BGmail%5D%2FImportant' -X 'SEARCH SUBJECT "Alton"'
 * curl -k -u tod.kurt@gmail.com:...  --url 'imaps://imap.gmail.com:993/INBOX/;UID=18'  - view email with id #18
 *
 *
 * Basic logic:
 * 1. Do initial search (unread, subject, or body); make a note of last UID
 * 2. On subsequent searches, check if oldest id is greater than saved
 */

#include "emailcurl.h"

#include <QUrl.h>



size_t Email::curlprocess(void* buffer,size_t size,size_t n,void *user)
{
    Email* email = (Email*)user;
    email->curlresponse += (char*)buffer;
    qDebug()<<"Email::curlprocess, buffer size:"<<size*n;
    //email->finishSlot();
    return size*n;
}

Email::Email(QString name,QObject *parent) :
    QThread(parent)
//    QObject(parent)
{
    simpleCrypt =  new SimpleCrypt(Q_UINT64_C(0xbf2fd4a2fcb9f00f));
    this->name=name;
    this->serverType = Email::IMAP;
    this->server="";
    this->username="";
    this->passwd="";
    this->port=0;
    this->ssl=true;
    this->mailbox="INBOX";
    this->searchString = "";
    this->searchType = Email::UNREAD;
    this->patternName="";
    this->freq=60;
    this->editing=false;
    this->lastMsgId = 0;
    this->lastMsgCount = 0;
    this->proxyUrl = "";
}

Email::~Email()
{
    if(simpleCrypt!=NULL)
        delete simpleCrypt;
}

void Email::run()
{
    errorsList.clear();  // maybe we don't need this?
    
    // construct imap URL for curl
    QString url;
    if( serverType == POP3 ) {
        url = (ssl) ? "pop3s://" : "pop3://";
        url += server + ":" + QString::number(port) + "/";
    }
    else {
        url = (ssl) ? "imaps://" : "imap://";
        url += server + ":" + QString::number(port) + "/";
        url += mailbox + "/";
    }
    // FIXME: Must uuencode malibox and other url bits
    qDebug() << "url: "<<url;
    
    curl = curl_easy_init();

    if( serverType == POP3 ) { 

    }
    else { // IMAP or GMAIL (imap)
        if( searchType == UNREAD ) { 
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH UNSEEN");  // "-X 'SEARCH UNSEEN'"
        }
        else if( searchType == SUBJECT ) {
            QString str = "SEARCH SUBJECT \""+ searchString + "\"";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, str.toUtf8().data() ); 
        }
        else if( searchType == SENDER ) {
            QString str = "SEARCH FROM \""+ searchString + "\"";
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, str.toUtf8().data() ); 
        }
    }
    // FIXME: how to add proxy?
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1 ); // testing
    // FIXME:
    if( proxyUrl != "" ) {
        curl_easy_setopt(curl, CURLOPT_PROXY, proxyUrl.toUtf8().data() );
        qDebug() << "emailcurl: setting proxy to" << proxyUrl;
    }
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    // FIXME: the above two options need a user checkbox so they agree
    curl_easy_setopt(curl, CURLOPT_USERNAME, username.toUtf8().data() );
    curl_easy_setopt(curl, CURLOPT_PASSWORD, passwd.toUtf8().data() );
    curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data() );
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlprocess );
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this );

    CURLcode res;

    res = curl_easy_perform(curl);
    
    QString newvalue="";
    
    if( res != CURLE_OK ) {
        qDebug() << "curl failed";
        qDebug() << "error: " << curl_easy_strerror(res);
        QString errstr = curl_easy_strerror(res);
        newvalue = "error: " + errstr;
    }
    else {
        qDebug() << "curl win!  curlresponse:\n" << curlresponse;

        if( serverType == POP3 ) {
            // do POP stuff
            addToLog("POP3 NOT IMPLEMENTED YET");
            newvalue = "error: POP disabled";
        }
        else { // assume IMAP
            int latestId = lastMsgId;
            int msgcount = 0;
            bool isTriggered = false;
            int searchNum = searchString.toInt();
            QRegularExpression re("^\\* SEARCH(( \\d+)*)"); // match the search results line
            QRegularExpression reids("( \\d+)");  // match only the id numbers in search results line
            QRegularExpressionMatch searchmatch = re.match( curlresponse );

            if( searchmatch.hasMatch() ) {  // "* SEARCH ..." matched, now check for message ids
                QString searchline = searchmatch.captured(0);
                QRegularExpressionMatchIterator idmatchi = reids.globalMatch( searchline );
                if( idmatchi.hasNext() ) {
                    while( idmatchi.hasNext() ) {
                        QRegularExpressionMatch match = idmatchi.next();
                        QString idm = match.captured(1);
                        int idn = idm.toInt();
                        if( idn > 0 ) {
                            latestId = idn;
                            msgcount++;
                        }
                    }
                    qDebug() << "checkMail: latestId:"<< latestId << ", msgcount:"<<msgcount;
                    
                    if( searchType == UNREAD ) {
                        int unreadnum = latestId - lastMsgId;
                        if( unreadnum == 0 ) {
                            newvalue = "No new unread messages";
                        }
                        else if( unreadnum >= searchNum ) {
                            //value = "UNREAD TRIGGER: " + QString::number(unreadnum);
                            newvalue="New Messages: "+QString::number(unreadnum);
                            lastMsgId = latestId;
                            isTriggered = true;
                        } else {
                            value = "Unread count: " + QString::number(unreadnum);
                        }
                    }
                    else if( searchType == SUBJECT ) {
                        int searchcount = msgcount - lastMsgCount;
                        int newcount = latestId - lastMsgId;
                        if( searchcount == 0 ) {
                            newvalue = "No new matching messages";
                        }
                        else if( searchcount > 0 ) {
                            if( newcount > 0 ) {
                                newvalue = "New Messages: "+ QString::number(searchcount);
                                lastMsgId = latestId;
                                lastMsgCount = msgcount;
                                isTriggered = true;
                            }
                            else { // matches but no new msgs
                                newvalue = "Matching Messages: "+ QString::number(searchcount);
                            }
                        }
                    }
                    else if( searchType == SENDER ) {
                        
                    }
                }
                else {
                    newvalue = "No Matching Messages";
                }
                
                emit updateOnlyTextStatus();
                setValue( newvalue );
                if( isTriggered ) {
                    //"New messages("+QString::number(matchingMessagesCount)+")",
                    emit addReceiveEvent(0, value, "Mail:"+name);
                    emit runPattern(patternName,false);
                }
            }
        } // else IMAP
    } // else CURLE_OK
    
    setValue( newvalue );
    qDebug() <<  "checkMail: last value = "<< value;
    
    curl_easy_cleanup(curl);
    curlresponse = ""; // zero out for next time
}

void Email::checkMail()
{
    addToLog("checking mail "+name+" ...");
    if(editing){
        addToLog(name+" in edit mode!");
        return;
    }

    if( !isRunning() ) {
        this->start();
    }
    //qDebug() << "checkMail end";
}


QJsonObject Email::toJson()
{
    QJsonObject obj;
    obj.insert("name", name);
    obj.insert("serverType",serverType);
    obj.insert("server",server);
    obj.insert("username",username);
    obj.insert("passwd",simpleCrypt->encryptToString(passwd));
    obj.insert("port",port);
    obj.insert("ssl",ssl);
    obj.insert("searchType", searchType);
    obj.insert("searchString", searchString);
    obj.insert("patternName",patternName);
    obj.insert("lastMsgId",lastMsgId);
    obj.insert("freq",freq);
    obj.insert("freqCounter",0);
    obj.insert("value",value);
    return obj;
}
// see: http://doc.qt.io/qt-5/qtcore-json-savegame-example.html
void Email::fromJson( QJsonObject obj)
{
    setName(obj.value("name").toString());
    setServerType( ServerType(qRound(obj["serverType"].toDouble())) );
    setServer( obj.value("server").toString());
    setUsername( obj.value("username").toString());
    setPasswd( simpleCrypt->decryptToString(obj.value("passwd").toString()));
    setPort(obj.value("port").toDouble());
    setSsl(obj.value("ssl").toBool());
    setSearchType( SearchType(qRound(obj["searchType"].toDouble())) );
    setSearchString( obj.value("searchString").toString() );
    setPatternName(obj.value("patternName").toString());
    setLastMsgId(obj.value("lastMsgId").toDouble());
    setFreq(obj.value("freq").toDouble());
    setFreqCounter(obj.value("freqCounter").toDouble());
    setValue(obj.value("value").toString());

}
// this is a hack on at least 3 levels
void Email::setProxySettings( QString proxyType, QString proxyHost, int proxyPort, QString proxyUser, QString proxyPass)
{
    if( proxyType == "socks5" || proxyType == "socks" ) {
        proxyType = "socks5";
    }
    else if( proxyType == "http" ) {
        proxyType = "http";
    }
    else {
        proxyType = "";
    }
    if( proxyType != "" ) {
        proxyUrl = proxyType+"://";
        if( proxyUser!="" && proxyPass!="" )
            proxyUrl += proxyUser+":"+proxyPass+"@";
        proxyUrl += proxyHost;
        if( proxyPort != 0 )
            proxyUrl += ":"+QString::number(proxyPort);
    } else {
        proxyUrl = "";
    }
}


void Email::setEditing(bool e)
{
    this->editing=e;
}

Email::ServerType Email::getServerType() {
    return serverType;
}
void Email::setServerType(ServerType type) {
    serverType = type;
}

QString Email::getName() {
    return name;
}
void Email::setName(QString name) {
    this->name=name;
}
QString Email::getServer() {
    return server;
}
void Email::setServer(QString server) {
    this->server=server;
}
QString Email::getUsername() {
    return username;
}
void Email::setUsername(QString user) {
    this->username=user;
}
QString Email::getPasswd() {
    return passwd;
}
void Email::setPasswd(QString passwd) {
    this->passwd=passwd;
}
int Email::getPort() {
    return port;
}
void Email::setPort(int port) {
    this->port=port;
}
bool Email::getSsl(){
    return ssl;
}
void Email::setSsl(bool ssl){
    this->ssl=ssl;
}

int Email::getFreq() {
    return freq;
}
void Email::setFreq(int freq) {
    this->freq=freq;
    //emit updateValues();
}
int Email::getFreqCounter(){
    return freqCounter;
}
void Email::setFreqCounter(int freqCounter){
    this->freqCounter=freqCounter;
    //emit updateValues();
}
// FIXME: wtf wat is this
void Email::changeFreqCounter(){
    if(freq==0) freq=3;
    freqCounter=(freqCounter+1)%freq;
}

Email::SearchType Email::getSearchType() {
    return searchType;
}
void Email::setSearchType(SearchType st) {
    this->searchType=st;
}
QString Email::getSearchString() {
    return searchString;
}
void Email::setSearchString(QString str) {
    this->searchString = str;
    //emit updateValues();
}

QString Email::getPatternName() {
    return patternName;
}
void Email::setPatternName(QString patternName) {
    this->patternName=patternName;
    emit updateValues();
}

QVariantList Email::getErrorsList(){
    QVariantList eList;
    for(int i=0;i<errorsList.count();i++){
        eList.append(QVariant(errorsList.at(i)));
    }
    return eList;
}
QString Email::getValue(){
    return value;
}
void Email::setValue(QString value){
    this->value=value;
    emit updateOnlyTextStatus();
}
int Email::getLastMsgId(){
    return lastMsgId;
}
void Email::setLastMsgId(int lastid){
    this->lastMsgId=lastid;
}


