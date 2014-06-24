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
    // try match json: '{"pattern":"my_pattern_name"}'
    QRegExp rx("\"?pattern\"?\\s*:\\s*\"(.+)\""); 
    if( rx.indexIn(str) != -1 ) { // match
        patt = rx.cap(1);
    }
    else {  // not json so try format: 'pattern: "my pattern name"'
        QRegExp rx2("pattern\\s*:\\s*(.+)");
        if( rx2.indexIn(str) != -1 ) {  // match
            patt = rx2.cap(1);
        }
    }
    //patt.replace("\"",""); // shouldn't capture doublequotes but does
    patt.remove(QRegExp("^\\s+\"*|\"*\\s+$")); // stupid qregexp doesn't allow non-greedy text capture
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
    QRegExp rx("(#[A-Fa-f0-9]{6})"); // look for "#cccccc" style hex colorcode
    if( rx.indexIn(str) != -1 ) { 
        qDebug() << "color match! " << rx.cap(1);
        c.setNamedColor( rx.cap(1) );
    }
    return c;
}
/**
 * Given a string, parse either a color pattern or color code
 * and trigger system based on that.
 *
 * @param str string to parse
 * @param type type of monitor running ("ifttt", "url", "file", "script")
 * @param lastModTime time parsing occurred
 */
void DataInput::parsePatternOrColor(QString str, QString type, int lastModTime)
{
    // look for pattern
    QString patternName = readColorPattern( str );
    qDebug() << "type:"<<type<< " patternName:"<<patternName << "str: "<<str;
    if( !patternName.isEmpty() ) { // pattern found
        emit runPattern(patternName, false);
        emit addReceiveEvent( lastModTime, patternName, type);
        input->setArg2(patternName);
    } 
    else {
        // or look for hex color
        QColor c = readColorCode( str );
        if( c.isValid() ) {  // color code found
            QString colorstr = c.name().toUpper();
            emit setColor( c );
            emit addReceiveEvent( lastModTime, colorstr, type);
            input->setArg2( colorstr );  // FIXME: arg2 should not be used for lastVal
        }
        else { 
            qDebug() << "parsePatternOrColor no color found";
        }
    }
}

void DataInput::start()
{
    QNetworkRequest nr;
    QString url;

    if( type == "ifttt" ) {
        url = "http://api.thingm.com/blink1/eventsall/" + iftttKey;
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
            emit setValueRet("Bad URL");
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
                    emit setValueRet("Old File"); // FIXME: is this signal ever used?
                    input->setArg2("OldFile");
                    input->setDate(-1);
                    emit toDelete(this);
                    return;
                }
                input->setDate( lastModTime); //fileInfo.lastModified().toTime_t());
                QString txt = "";
                QTextStream in(&f);
                txt.append(in.readAll());
                
                parsePatternOrColor( txt, type, lastModTime );
            } // last modified
        }
        emit toDelete(this);
    }
    else if( type == "script" ) { 
        //QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, input->arg1());
        QFileInfo fileInfo;
        fileInfo.setFile( input->arg1() );
        if( !fileInfo.exists() ) {
            emit setValueRet("Not Found"); // FIXME: why this?
            input->setArg2("Not Found");
            input->setDate(-1);
            emit toDelete(this);
        } 
        else if( !fileInfo.isExecutable() ) { 
            emit setValueRet("Not Executable"); // FIXME: why this?
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
        qDebug() << "datainput:start: none type";
    }
    else { 
        qDebug() << "datainput:start: bad type! should never get called";
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
            lastModTime = 0; // put as far back in time as possible
        }
        if( lastModTime <= input->date() ) {  // old page
            qDebug() << "onFinished: old url";
            // FIXME: do something better here
            input->setArg2( "Not Modified" );  // FIXME: arg2 should not be used for lastVal
        }
        else { 
            input->setDate(lastModTime); // FIXME: blinkinput vs datainput? which is which, omg marcin, really?

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
        input->setArg2("connect error");
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

