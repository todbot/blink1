/**
 * DataInput -- 
 * 
 * Handles network or process aspects of IFTTT, URL, File, and Script monitors
 * (still not sure why this isn't part of Blink1Input)
 *
 */

#include "datainput.h"
#include <QDir>
#include <QStandardPaths>
#include <QRegularExpression>

DataInput::DataInput(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    processOutput = "";
}

DataInput::~DataInput()
{
    delete networkManager;
}

/**
 * Search for color pattern in given string
 * contentStr can also be JSON
 * returns pattern name if successful, or 'nil' if no pattern found
 * can be in form:
 *   pattern: policecar 
 *   pattern: "happy color dance"
 *   { "pattern": "red flashes" }
 * 
 * @param str string to parse
 * @return parsed color pattern name or empty string if no match
 */
QString DataInput::readColorPattern(QString str)
{
    QString patt;
    QRegularExpression   re("\"?pattern\"?:\\s*((\"(.+)\")|((.+)\\s))");
    QRegularExpressionMatch match = re.match(str);
    if( match.hasMatch() ) {
        patt = match.captured( match.lastCapturedIndex() );
    }
    return patt;
}
/**
 * Read potential color code in string
 * @param str string to parse
 * @return valid QColor or invalid QColor if parsing failed
 */
QColor DataInput::readColorCode(QString str)
{
    QColor c;
    QRegularExpression re("(#[A-Fa-f0-9]{6})"); // look for "#cccccc" style hex colorcode
    QRegularExpressionMatch match = re.match(str);
    if( match.hasMatch() ) {
        c.setNamedColor( match.captured( match.lastCapturedIndex()) );
    }
    return c;
}
/**
 * Given a string, parse either a color pattern or color code
 * and trigger system based on that.
 * 
 * FIXME: this knows too much about emitting signals
 *
 * @param str string to parse
 * @param type type of monitor running ("ifttt", "url", "file", "script")
 * @param lastModTime time parsing occurred
 * @return bool true if match was found, false if not
 */
bool DataInput::parsePatternOrColor(QString str, QString type, int lastModTime)
{
    bool success = false;
    // look for pattern
    QString patternName = readColorPattern( str );
    bool patternFound = !patternName.isEmpty();
    qDebug() << "type:"<<type<< " patternName:"<<patternName << "str: "<<str;
    if( patternFound ) {
        emit runPattern(patternName, false);
        emit addReceiveEvent( lastModTime, patternName, type);
        input->setArg2(patternName);
        success = true;
    } 
    // or look for hex color
    else { 
        QColor c = readColorCode( str );
        bool colorFound = c.isValid();
        if( colorFound ) { 
            QString colorstr = c.name().toUpper();
            emit setColor( c );
            emit addReceiveEvent( lastModTime, colorstr, type);
            input->setArg2( colorstr );  // FIXME: arg2 should not be used for lastVal
            success = true;
        }
        else { 
            qDebug() << "parsePatternOrColor no color found";
        }
    }
    return success;
}

void DataInput::start()
{
    QNetworkRequest nr;
    QString url;
    //qDebug() << "DataInput:start:"<<type<<":"<<input->arg1();

    if( type == "ifttt" ) {
        // url = "http://api.thingm.com/blink1/eventsall/" + iftttKey;
        url = "http://feed.thingm.com/blink1/eventsall/" + iftttKey;
        //url = "http://localhost:3232/blink1/eventsall/" + iftttKey;
        nr.setUrl(QUrl(url));
        reply = networkManager->get(nr);
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError()));
    }
    else if( type == "url" ) { 
        url = input->arg1();
        //qDebug() << "datainput:start url: "<<url; 
        if(!url.startsWith("http://") && !url.startsWith("https://"))
            url="http://"+url;
        QUrl correctUrl(url);
        if(correctUrl.isValid()) {
            nr.setUrl(QUrl(url));
            reply = networkManager->get(nr);
            connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError()));
        }
        else {
            input->setArg2("Bad URL");
            input->setDate(-1);  // FIXME: don't like -1 here
            emit toDelete(this);
        }
    }
    else if( type == "file" ) { 
        QFileInfo fileInfo;
        fileInfo.setFile(input->arg1());
        if( !fileInfo.exists() ) {
            qDebug() << "datainput:start: no file";
            input->setArg2("Not Found");
            input->setDate(-1);
        }
        else { 
            int lastModTime = fileInfo.lastModified().toTime_t(); // why was cast to uint?
            if( lastModTime > input->date()) {
                qDebug() << "datainput:start: file newer";
                QFile f(input->arg1());
                if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    input->setArg2("Couldn't Open");
                    input->setDate(-1);  // FIXME: why -1? what does it mean?
                    emit toDelete(this);
                    return;
                }
                input->setDate( lastModTime); //fileInfo.lastModified().toTime_t());
                QString txt = "";
                QTextStream in(&f);
                txt.append(in.readAll());

                bool good = parsePatternOrColor( txt, type, lastModTime );
                if( !good ) { 
                    input->setArg2("Bad Parse");
                }
            } // last modified
            else { 
                //input->setArg2("Old File");  // FIXME: should do something to indicate older file
                //input->setDate(-1);
            }
        }
        emit toDelete(this);
    }
    else if( type == "script" ) { 
        //QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, input->arg1());
        QFileInfo fileInfo;
        fileInfo.setFile( input->arg1() );
        if( !fileInfo.exists() ) {
            input->setArg2("Not Found");
            input->setDate(-1);
            emit toDelete(this);
        } 
        else if( !fileInfo.isExecutable() ) { 
            input->setArg2("Not Executable");
            input->setDate(-1);
            emit toDelete(this);
        }
        else { 
            // FIXME: should check new value compare to lastVal
            // (and FIXME: need to refactor to properly use lastVal for all monitor types)
            //if(fileInfo.lastModified().toTime_t() != (uint)input->date()){
            // no, don't do lastModTime check on exec file, jeez
            input->setDate(fileInfo.lastModified().toTime_t());
            process = new QProcess;
            connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessOutput()));
            connect(process, SIGNAL(readyReadStandardError()), this, SLOT(onError()));
            connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError()));
            connect(process, SIGNAL(finished(int)), this, SLOT(onProcessFinished()));
            // start process running
            process->start( fileInfo.canonicalFilePath() );
        }
    }
    else if( type == "none" ) { 
        //qDebug() << "datainput:start: none type";
    }
    else { 
        qDebug() << "datainput:start: bad type! should never get called; type="<<type;
        emit toDelete(this);
    }
}


void DataInput::onFinished()
{
    int lastModTime;
    QString txt;
    
    if( type == "ifttt" ) {  
        txt = reply->readAll();
        QString dateString = "";  // FIXME: what is dateString for?
        if(input->type() == "ifttt") {  // FIXME: why checking type again?
            emit iftttToCheck(txt);  // FIXME: why is sending a signal instead of doing what URL & FILE do?
        }
    }
    else if( type == "url" ) { 
        txt = reply->readAll();
        QDateTime lastModified = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
        lastModTime = lastModified.toTime_t();
        qDebug() << "onFinished: url finished: lastMod:"<< lastModTime << " txt: " << txt;
        
        if( lastModTime == -1 ) { // missing lastmodified
            lastModTime = input->date() + 10 ; // so fake it
        }
        if( lastModTime <= input->date() ) {  // old page
            qDebug() << "onFinished: old url";
            // FIXME: do something better here
            input->setArg2( "Not Modified" );  // FIXME: arg2 should not be used for lastVal
        }
        else { 
            input->setDate(lastModTime); // FIXME: blinkinput vs datainput? which is which,omg marcin,really?
            parsePatternOrColor( txt, type, lastModTime);
        }
    }
    else { 
        qDebug() << "onFinished: unexpected type: "<<type;
    }

    delete reply;
    emit toDelete(this);
}


void DataInput::onProcessOutput()
{
    readingProcess = true;
    processOutput.append(process->readAll());
    readingProcess = false;
}

// this is for scripts?
void DataInput::onProcessFinished()
{
    qDebug() << "datainput::onProcessFinished";
    while(readingProcess)
        QThread::usleep(200);
    process->close();
    process->deleteLater();

    parsePatternOrColor( processOutput, type, QDateTime::currentDateTime().toTime_t() );

    emit toDelete(this);
}

void DataInput::onError()
{
    if( type == "ifttt" ) { 
        // FIXME: maybe don't report this because we do it periodically?
        //input->setArg2("connect error");
    }
    else if( type == "url") {
        qDebug() << " error on URL " << reply;
        input->setArg2("bad URL"); // FIXME: hack
        delete reply;
    }
    else if(type == "script") {
        qDebug() << "datainput: script error:";
        //qDebug() << process->errorString();
        process->deleteLater();  // FIXME: what is deleteLater() for?
//        while(process)
        QThread::usleep(200);  // FIXME: why is this here?
    }
    emit toDelete(this);
}

