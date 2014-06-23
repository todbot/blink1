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
    responseTo=NULL;
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
 * @param type type of monitor running ("IFTTT", "URL", "FILE", "SCRIPT")
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

    if( type == "IFTTT" ) {
        url = "http://api.thingm.com/blink1/eventsall/" + iftttKey;
        nr.setUrl(QUrl(url));
        reply = networkManager->get(nr);
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError()));
    }
    else if( type == "URL" ) { 
        url = input->arg1();
        qDebug() << "url: "<<url; 
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
    else if( type == "FILE" ) { 
        QFileInfo fileInfo;
        fileInfo.setFile(input->arg1());
        if( !fileInfo.exists() ) {
            qDebug() << "no file";
            input->setArg2("Not Found");
            input->setDate(-1);
        }
        else { 
            int lastModTime = fileInfo.lastModified().toTime_t();
            //if( lastModTime != (uint)input->date()) {  // why cast to uint?
            if( lastModTime != input->date()) {
                qDebug() << "newer file";
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
                
                parsePatternOrColor( txt, "FILE", lastModTime );
            } // last modified
        }
        emit toDelete(this);
    }
    else if( type == "SCRIPT" ) { 
        //QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, input->arg1());
        //QFile f(path);
        //fileInfo.setFile(path);
        QFileInfo fileInfo;
        fileInfo.setFile( input->arg1() );
        if( !fileInfo.exists() ) {
            emit setValueRet("Not Found"); // FIXME: why this?
            input->setArg2("Not Found");
            input->setDate(-1);
            emit toDelete(this);
        } 
        else { 
            // FIXME: should check new value compare to lastVal
            // (and FIXME: need to refactor to properly use lastVal for all monitor types)
            //if(fileInfo.lastModified().toTime_t() != (uint)input->date()){
            input->setDate(fileInfo.lastModified().toTime_t());
            process = new QProcess;
            connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessOutput()));
            connect(process, SIGNAL(readyReadStandardError()), this, SLOT(onError()));
            connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError()));
            connect(process, SIGNAL(finished(int)), this, SLOT(onProcessFinished()));
            process->start( fileInfo.canonicalFilePath() );  // start process running
            //} else {
            //emit toDelete(this);
            //}
        }
    }
    else { 
        qDebug() << "bad type!: should never get called";
        emit toDelete(this);
    }
}


void DataInput::onFinished()
{
    int lastModTime;
    QString txt;

    if( type == "IFTTT" ) {  
        txt = reply->readAll();
        QString dateString = "";
        if(input->type().toUpper() == "IFTTT") {
            if(responseTo==NULL) 
                emit iftttToCheck(txt);  // FIXME: why is sending a signal instead of doing what URL & FILE do?
            else 
                emit iftttToCheck(txt,input);  // FIXME: wtf?
        }
    }
    else if( type == "URL" ) { 
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

            parsePatternOrColor( txt, "URL", lastModTime);
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

    parsePatternOrColor( processOutput, "SCRIPT", QDateTime::currentDateTime().toTime_t() );

    emit toDelete(this);
}

void DataInput::onError()
{
    if( type == "IFTTT" ) { 
        
    }
    else if(type == "SCRIPT") {
        //qDebug() << "Script error:";
        //qDebug() << process->errorString();
        process->deleteLater();
//        while(process)
            QThread::usleep(200);
    }
    else if( type == "URL") {
        qDebug() << " error on URL " << reply;
        input->setArg2("bad URL"); // FIXME: hack
        delete reply;
    }
    emit toDelete(this);
}

// script
    /*
    int idx = processOutput.indexOf(QRegExp("#([0-9a-fA-F]{6})"));
    QColor c=QColor("#000000");
    if(idx!=-1)
    {
        c=QColor(processOutput.mid(idx,7));
        emit setColor(c);
        emit addReceiveEvent(QDateTime::currentDateTime().toTime_t(), c.name().toUpper(), "SCRIPT");
        input->setArg2(c.name());
    }
    else
    {
        bool found = false;
        int currentIndex = 0;
        int idx2;
        QString pName;
        while(!found)
        {
            idx = processOutput.indexOf("\"", currentIndex);
            if(idx == -1)
                break;
            currentIndex = idx+1;
            idx2 = processOutput.indexOf("\"", currentIndex);
            if(idx2 == -1)
                break;
            currentIndex = idx2+1;

            pName = processOutput.mid(idx+1, idx2-idx-1);
            for(int i=0; i<patternList.count(); i++)
            {
                if(patternList[i] == pName)
                {
                    emit runPattern(pName, false);
                    emit addReceiveEvent(QDateTime::currentDateTime().toTime_t(), pName, "SCRIPT");
                    input->setArg2(pName);
                    found = true;
                    break;
                }
            }

        }
        if(!found)
            input->setArg2("NO VALUE");
    }
*/

// file
            /*
            int idx=txt.indexOf(QRegExp("#([0-9a-fA-F]{6})"));
            QColor c=QColor("#000000");
            f.close();
            if(idx!=-1)
            {
                c=QColor(txt.mid(idx,7));
                emit setColor(c);
                emit addReceiveEvent(fileInfo.lastModified().toTime_t(), c.name().toUpper(), "FILE");
                input->setArg2(c.name());
            }
            else
            {
                QStringList list;
                list.append("pattern:"); list.append("\"pattern\":");
                QString patternName;
                int index=-1;
                for(int i=0; i<list.count(); i++) {
                    index = txt.indexOf(list.at(i),0);
                    if(index != -1)
                        break;
                }
                if(index != -1) {
                    index = txt.indexOf(":", index);
                    int idx1, idx2;
                    idx1 = txt.indexOf("\"", index);
                    idx2 = txt.indexOf("\"", idx1+1);
                    patternName = txt.mid(idx1+1, idx2-idx1-1);
                    for(int i=0; i<patternList.count(); i++) {
                        if(patternList[i] == patternName) {
                            emit runPattern(patternName, false);
                            emit addReceiveEvent(fileInfo.lastModified().toTime_t(), patternName, "FILE");
                            input->setArg2(patternName);
                            break;
                        }
                    }

                }else{
                    input->setArg2("NO VALUE");
                }
            }
        */

