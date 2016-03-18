#ifndef EMAILCURL_H
#define EMAILCURL_H

#include <QObject>
//#include "simplecrypt.h"
#include <QJsonObject>
#include <QStringList>
#include <QLabel>
#include <QTimer>

#include <QFile>
#include <QTextStream>
#include <QtGui>

#include "simplecrypt.h"
#include "curl/curl.h"

//class Email : public QObject
class Email : public QThread
{
    Q_OBJECT
    
    Q_ENUMS(ServerType);
    Q_ENUMS(SearchType);
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY updateValues)
    Q_PROPERTY(ServerType serverType READ getServerType WRITE setServerType NOTIFY updateValues)
    Q_PROPERTY(QString server READ getServer WRITE setServer NOTIFY updateValues)
    Q_PROPERTY(QString username READ getUsername WRITE setUsername NOTIFY updateValues)
    Q_PROPERTY(QString passwd READ getPasswd WRITE setPasswd NOTIFY updateValues)
    Q_PROPERTY(int port READ getPort WRITE setPort NOTIFY updateValues)
    Q_PROPERTY(bool ssl READ getSsl WRITE setSsl NOTIFY updateValues)
    Q_PROPERTY(SearchType searchType READ getSearchType WRITE setSearchType NOTIFY updateValues)
    Q_PROPERTY(QString searchString READ getSearchString WRITE setSearchString NOTIFY updateValues)
    Q_PROPERTY(QString patternName READ getPatternName WRITE setPatternName NOTIFY updateValues)
    Q_PROPERTY(int freq READ getFreq WRITE setFreq NOTIFY updateValues)
    Q_PROPERTY(QVariantList getErrorsList READ getErrorsList NOTIFY errorsUpdate)
    Q_PROPERTY(QString value READ getValue WRITE setValue NOTIFY updateOnlyTextStatus)

public:
    
    enum ServerType {
        IMAP  = 0,
        POP3  = 1,
        GMAIL = 2,
    };
    
    enum SearchType {
        UNREAD = 0,
        SUBJECT,
        SENDER,
        BODY
    };
    
    // Do not forget to declare your class to the QML system.
    //static void declareQML() {
    //    qmlRegisterType<Email>("ServerType", 1, 0, "Style");
    //}

    explicit Email(QString name, QObject *parent = 0);
    ~Email();
    
    static size_t curlprocess(void* buffer,size_t size,size_t n,void *user);

    QJsonObject toJson();
    void fromJson( QJsonObject obj);
    QString getName();
    void setName(QString name);
    QString getServer();
    void setServer(QString server);
    int getPort();
    void setPort(int port);
    QString getUsername();
    void setUsername(QString user);
    QString getPasswd();
    void setPasswd(QString pass);
    Email::ServerType getServerType();
    void setServerType(ServerType type);
    bool getSsl();
    void setSsl(bool ssl);

    QString getSearchString();
    void setSearchString(QString str);
    Email::SearchType getSearchType();
    void setSearchType(Email::SearchType st);

    QString getPatternName();
    void setPatternName(QString patternName);

    int getFreq();
    void setFreq(int freq);
    int getFreqCounter();
    void setFreqCounter(int freqCounter);
    void changeFreqCounter();

    // "value" is status from last email check 
    QString getValue();
    void setValue(QString value);
    int getLastMsgId();
    void setLastMsgId(int lastid);

    void setProxySettings( QString proxyType, QString proxyHost, int proxyPort, QString proxyUser, QString proxyPass);

signals:
    void runPattern(QString pattname,bool fromQml); // emitted when match happens and pattern should play
    void addToLog(QString txt);  // emit a log ine, connects to mainwindow.addToLog for logging
    void addReceiveEvent(int date, QString name, QString from); // emitted to add a line to recent events
    void updateValues(); // unused?
    void errorsUpdate(); // unused?
    void updateOnlyTextStatus(); // unused?
        
public slots:
    void checkMail(); // public, called by QML & mainwindow
    QVariantList getErrorsList();  // public, called by QML 
    void setEditing(bool e); // public, called by mainwindow 

private:
    QString name;
    ServerType serverType;
    QString server;
    QString username;
    QString passwd;
    QString mailbox;
    int port;
    bool ssl;
    QString searchString;
    SearchType searchType;
    QString patternName;
    int freq;
    int freqCounter;  // FIXME: wat
    bool editing;
    SimpleCrypt *simpleCrypt;

    QStringList errorsList;  // for QML
    QString value;    // for QML

    QString curlresponse;
    CURL*  curl;
    int lastMsgId;
    int lastMsgCount;

    QString proxyUrl;

protected:
    void run();
    
};

#endif // EMAILCURL_H
