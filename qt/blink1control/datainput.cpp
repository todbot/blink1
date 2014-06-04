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


void DataInput::start()
{
    QNetworkRequest nr;
    QString url;
    int typeNumber = typeToInt(type);
    switch(typeNumber)
    {
    case 0: {
        url = "http://api.thingm.com/blink1/eventsall/" + iftttKey;
        nr.setUrl(QUrl(url));
        reply = networkManager->get(nr);
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError()));
    }
        break;

    case 1: {
        url = input->arg1();
        if(!url.startsWith("http://") && !url.startsWith("https://"))
            url="http://"+url;
        QUrl correctUrl(url);
        if(correctUrl.isValid())
        {
            nr.setUrl(QUrl(url));
            reply = networkManager->get(nr);
            connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError()));
        }
        else
        {
            emit setValueRet("NOT FOUND");
            input->setArg2("NOT FOUND");
            input->setDate(-1);
            emit toDelete(this);
        }
    }
        break;

    case 2: {
        QFileInfo fileInfo;
        fileInfo.setFile(input->arg1());
        if(!fileInfo.exists()){
            input->setArg2("NOT FOUND");
            input->setDate(-1);
        }
        if(fileInfo.lastModified().toTime_t() != (uint)input->date()) {
            QFile f(input->arg1());
            if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                emit setValueRet("NOT FOUND");
                input->setArg2("NOT FOUND");
                input->setDate(-1);
                emit toDelete(this);
                return;
            }
            input->setDate(fileInfo.lastModified().toTime_t());
            QString txt = "";
            QTextStream in(&f);
            txt.append(in.readAll());
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
        }
        emit toDelete(this);
    }
        break;

    case 3: {
        process = new QProcess;
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessOutput()));
        connect(process, SIGNAL(readyReadStandardError()), this, SLOT(onError()));
        connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError()));
        connect(process, SIGNAL(finished(int)), this, SLOT(onProcessFinished()));
        QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, input->arg1());
        QFile f(path);
        QFileInfo fileInfo;
        fileInfo.setFile(path);
        if(f.exists()){
            if(fileInfo.lastModified().toTime_t() != (uint)input->date()){
                input->setDate(fileInfo.lastModified().toTime_t());
                process->start(path);
            }else{
                emit toDelete(this);
            }
        }else
        {
            emit setValueRet("NOT FOUND");
            input->setArg2("NOT FOUND");
            input->setDate(-1);
            emit toDelete(this);
        }
    }
        break;

    default: {
        emit toDelete(this);
    }
        break;
    }
}

void DataInput::onFinished()
{
    int date;
    QString txt;

    int typeNumber = typeToInt(type);
    switch(typeNumber)
    {
    case 0: {
        txt = reply->readAll();
        QString dateString = "";
        if(input->type().toUpper() == "IFTTT.COM") {

            if(responseTo==NULL) emit iftttToCheck(txt); else emit iftttToCheck(txt,input);
        }
    }
        break;

    case 1: {
        QDateTime dt = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
        date = dt.toTime_t();
        if(date != -1) {
            if(input->date() != date) {
                input->setDate(date);

                txt = reply->readAll();
                int idx=txt.indexOf(QRegExp("#([0-9a-fA-F]{6})"));
                QColor c=QColor("#000000");
                if(idx!=-1)
                {
                    c=QColor(txt.mid(idx,7));
                    emit setColor(c);
                    emit addReceiveEvent(date, c.name().toUpper(), "URL");
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

                        for(int i=0; i<patternList.count(); i++)
                        {
                            if(patternName == patternList[i]) {
                                emit runPattern(patternName, false);
                                emit addReceiveEvent(date, patternName, "URL");
                                input->setArg2(patternName);
                                break;
                            }
                        }
                    }else{
                        input->setArg2("NO VALUE");
                    }
                }
            }
        }else{
            input->setArg2("NO VALUE");
        }
    }
        break;
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
void DataInput::onProcessFinished()
{
    while(readingProcess)
        QThread::usleep(200);
    process->close();
    process->deleteLater();
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
    emit toDelete(this);
}

void DataInput::onError()
{
    if(type == "SCRIPT")
    {
        //qDebug() << "Script error:";
        //qDebug() << process->errorString();
        process->deleteLater();
//        while(process)
            QThread::usleep(200);
    }
    else
    {
        delete reply;
    }
    emit toDelete(this);
}

int DataInput::typeToInt(QString type)
{
    if(type == "IFTTT.COM")
        return 0;
    if(type == "URL")
        return 1;
    if(type == "FILE")
        return 2;
    if(type == "SCRIPT")
        return 3;
    return -1;
}

