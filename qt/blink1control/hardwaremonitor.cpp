#include "hardwaremonitor.h"

HardwareMonitor::HardwareMonitor(QString name,QObject *parent) :
    QObject(parent)
{
    this->name=name;
    this->lvl=50;
    this->action=0;
    this->type=0;
    this->patternName="";
    this->freq=60;
    this->value=0;
    this->freqCounter=1;
    this->done=false;
    this->role=0;
    this->status="NO VALUE";
    this->p=NULL;
    this->p2=NULL;
    this->p3=NULL;
    this->editing=false;
}
void HardwareMonitor::checkMonitor(){
    if(editing){
        return;
    }
    status="checking...";
    emit updateOnlyStatusAndCurrentValue();
    if(type==0){
        checkBattery();
    }else if(type==1){
        checkCpu();
    }else if(type==2){
        checkRam();
    }
}
void HardwareMonitor::checkBattery(){
#ifdef Q_OS_WIN
    SYSTEM_POWER_STATUS stat;
    GetSystemPowerStatus(&stat);
    int life = stat.BatteryLifePercent;
    int BTime = stat.BatteryLifeTime;
    qDebug()<<stat.ACLineStatus;  //1-charging 0-no charging
    qDebug()<<stat.BatteryFlag;   //1- battery, 128-no battery
    addToLog(QString(stat.ACLineStatus));
    addToLog(QString(stat.BatteryFlag));
    bool battery="true";
    if(stat.BatteryFlag==128) battery=false;
    if(stat.BatteryFlag!=128) {
        qDebug()<<life << "% and " << BTime << "Battery Life!\n";
        addToLog(QString::number(life) + "% and " + QString::number(BTime) + "Battery Life!");
    }
    value=life;

    if(!battery){
        value=-1;
        status="NO BATTERY";
        done=false;
    }else{
        if(role==0 || role==2){
            extraValue=(BTime!=-1)?BTime:0;
            status=QString::number(value)+"%";
        }else{
            status="Level not reached.";
        }
        if(checkValue(value)){
            if(!done){
                if(role>0){
                    emit addReceiveEvent(0,QString::number(value)+"% Battery",name);
                    emit runPattern(patternName,false);
                }
            }
            if(role==1)
                status="Level reached!";
            done=true;
        }else{
            done=false;
        }
    }
    emit updateOnlyStatusAndCurrentValue();
#endif
#ifdef Q_OS_MAC
    if(p3!=NULL) return;
    p3=new QProcess();
    p3->start("pmset -g batt");
    connect(p3,SIGNAL(finished(int)),this,SLOT(readyReadBattery(int)));
#endif
}

void HardwareMonitor::readyReadBattery(int st){
#ifdef Q_OS_MAC
    bool battery=true;
    bool correct=true;
    int BTime;
    if(st==0){
        QString tmp2=p3->readAllStandardOutput();
        qDebug()<<tmp2;
        QStringList pom2=tmp2.split(QRegExp("(  +|\r\n|\n|;)"));
        qDebug()<<pom2;
        if(pom2.count()>1){
            QString source=pom2.at(0).mid(pom2.at(0).indexOf("\'")+1);
            source=source.remove(source.length()-1,1);
            qDebug()<<"Source "<<source;
            addToLog("Source "+source);
            if(!pom2.at(1).contains("InternalBattery")){
                qDebug()<<"no battery";
                addToLog("no battery");
                battery=false;
            }
            if(battery){
                if(pom2.count()>3){
                    qDebug()<<"Battery status "<<pom2.at(2);
                    addToLog("Battery status "+pom2.at(2));
                    qDebug()<<pom2.at(3);
                    qDebug()<<"Time: "<<pom2.at(4);
                    addToLog("Time: "+pom2.at(4));
                    BTime=pom2.at(4).toInt();
                    value=(pom2.at(2).left(pom2.at(2).length()-1)).toInt();
                }else{
                    correct=false;
                }
            }
        }else{
            correct=false;
        }
    }
    if(st==0 && correct){
        if(!battery){
            value=-1;
            status="NO BATTERY";
            done=false;
        }else{
            if(role==0 || role==2){
                extraValue=(BTime!=-1)?BTime:0;
                status=QString::number(value)+"%";
            }else{
                status="Level not reached.";
            }
            if(checkValue(value)){
                if(!done){
                    if(role>0){
                        emit addReceiveEvent(0,QString::number(value)+"% Battery",name);
                        emit runPattern(patternName,false);
                    }
                }
                if(role==1)
                    status="Level reached!";
                done=true;
            }else{
                done=false;
            }
        }
    }else{
        status="Internal error";
        done=false;
        freqCounter=0;
    }
    emit updateOnlyStatusAndCurrentValue();
    p3->close();
    disconnect(p3);
    p3=NULL;
#endif
}

void HardwareMonitor::checkCpu(){
#ifdef Q_OS_WIN
    if(p!=NULL) return;
    p=new QProcess();
    p->start("wmic cpu get loadpercentage");
    connect(p,SIGNAL(finished(int)),this,SLOT(readyReadCpu(int)));
#endif
#ifdef Q_OS_MAC
    if(p!=NULL) return;
    p=new QProcess();
    p->start("sh -c \"top -l 1 | grep 'CPU usage:'\"");//"top -o cpu -l 1");
    connect(p,SIGNAL(finished(int)),this,SLOT(readyReadCpu(int)));
#endif
}
void HardwareMonitor::readyReadCpu(int st){
    bool correct=true;
#ifdef Q_OS_WIN
    if(st==0){
        QString tmpp=p->readAllStandardOutput();
        tmpp=tmpp.simplified();
        QStringList pomm=tmpp.split(QRegExp("( +|\r\n)"));
        if(pomm.count()>0){
            qDebug()<<pomm.at(1).toInt();
            qDebug()<<"Status: "<<st;
            addToLog(pomm.at(1));
            addToLog("Status: "+QString::number(st));
            value=pomm.at(1).toInt();
        }else{
            correct=false;
        }
    }
#endif
#ifdef Q_OS_MAC
    if(st==0){
        QString tmp=p->readAllStandardOutput();
        tmp=tmp.remove(",");
        QStringList pom=tmp.split(QRegExp("( +|\r\n|\n)"));
        qDebug()<<pom;
        int cpuMonitor=pom.indexOf("user");
        if(cpuMonitor!=-1){
            QString cpuUsage=pom.at(cpuMonitor-1);
            cpuUsage=cpuUsage.left(cpuUsage.indexOf("%"));
            qDebug()<<"CPU usage "<<cpuUsage.toDouble()<<"%";
            addToLog("CPU usage "+cpuUsage+"%");
            value=cpuUsage.toDouble();
        }else{
            correct=false;
        }
    }
#endif
    if(st==0 && correct){
        if(role==0 || role==2){
            status=QString::number(value)+"%";
        }else{
            status="Level not reached!";
        }
        if(checkValue(value)){
            if(!done){
                if(role>0){
                    emit addReceiveEvent(0,QString::number(value)+"% CPU",name);
                    emit runPattern(patternName,false);
                }
            }
            if(role==1)
                status="Level reached!";
            done=true;
        }else{
            done=false;
        }
    }else{
        status="Internal error";
        done=false;
        freqCounter=0;
    }
    emit updateOnlyStatusAndCurrentValue();
    p->close();
    disconnect(p);
#ifdef Q_OS_WIN
    delete p;
#endif
    p=NULL;
}

void HardwareMonitor::checkRam(){
#ifdef Q_OS_WIN
    if(p2!=NULL) return;
    p2=new QProcess();
    p2->start("wmic OS get FreePhysicalMemory,TotalVisibleMemorySize");
    connect(p2,SIGNAL(finished(int)),this,SLOT(readyReadRam(int)));
#endif
#ifdef Q_OS_MAC
    if(p2!=NULL) return;
    p2=new QProcess();
    p2->start("sh -c \"top -o cpu -l 1 | grep 'PhysMem:'\"");
    connect(p2,SIGNAL(finished(int)),this,SLOT(readyReadRam(int)));
#endif
}
void HardwareMonitor::readyReadRam(int st){
    bool correct=true;
#ifdef Q_OS_WIN
    if(st==0){
        QString tmp=p2->readAllStandardOutput();
        QStringList pom=tmp.split(QRegExp("(  +|\r\n)"));
        qDebug()<<pom;
        if(pom.count()>3){
            double total=pom.at(4).toDouble()*1.0/(1024);
            double used=pom.at(3).toDouble()*1.0/(1024);
            qDebug()<<100.0-used*1.0/total*100<<"%";
            qDebug()<<st;
            value=100.0-used*1.0/total*100;
            addToLog(QString::number(value)+"%");
            addToLog(QString::number(st));
        }else{
            correct=false;
        }
    }
#endif
#ifdef Q_OS_MAC
    if(st==0){
        QString tmp=p2->readAllStandardOutput();
        tmp=tmp.remove(",");
        tmp=tmp.remove(".");
        QStringList pom=tmp.split(QRegExp("( +|\r\n|\n)"));
        //qDebug()<<pom;
        int ramMonitor=pom.indexOf("used");
        int freeM=pom.indexOf("free");
        if(freeM==-1){
            freeM=pom.indexOf("unused");
        }
        if(ramMonitor!=-1 && freeM!=-1){
            QString used=pom.at(ramMonitor-1);
            int v=used.indexOf(QRegExp("[a-zA-Z]"));
            long long int nused=used.left(v).toLongLong();            
            QString vv=used.mid(v);
            if(vv=="M") nused*=1024*1024;
            else if(vv=="K") nused*=1024;
            else if(vv=="G") nused*=1024*1024*1024;

            QString free=pom.at(freeM-1);
            v=free.indexOf(QRegExp("[a-zA-Z]"));
            long long int nused2=free.left(v).toLongLong();
            vv=free.mid(v);
            if(vv=="M") nused2*=1024*1024;
            else if(vv=="K") nused2*=1024;
            else if(vv=="G") nused2*=1024*1024*1024;

            qDebug()<<"RAM usage "<<nused*1.0/(nused+nused2)*100.0<<"%";
            value=nused*1.0/(nused+nused2)*100.0;
            addToLog("RAM usage "+QString::number(value)+"%");
        }else{
            correct=false;
        }
    }
#endif
    if(st==0 && correct){
        if(role==0 || role==2){
            status=QString::number(value)+"%";
        }else{
            status="Level not reached!";
        }
        if(checkValue(value)){
            if(!done){
                if(role>0){
                    emit addReceiveEvent(0,QString::number(value)+"% RAM",name);
                    emit runPattern(patternName,false);
                }
            }
            if(role==1)
                status="Level reached!";
            done=true;
        }else{
            done=false;
        }
    }else{
        status="Internal error";
        done=false;
        freqCounter=0;
    }
    emit updateOnlyStatusAndCurrentValue();
    p2->close();
    disconnect(p2);
#ifdef Q_OS_WIN
    delete p2;
#endif
    p2=NULL;
}
QString HardwareMonitor::getName(){
    return name;
}
void HardwareMonitor::setName(QString name){
    this->name=name;
}
int HardwareMonitor::getType(){
    return type;
}
void HardwareMonitor::setType(int type){
    this->type=type;
    emit updateValues();
}
int HardwareMonitor::getLvl(){
    return lvl;
}
void HardwareMonitor::setLvl(int lvl){
    this->lvl=lvl;
}
int HardwareMonitor::getAction(){
    return action;
}
void HardwareMonitor::setAction(int action){
    this->action=action;
}
QString HardwareMonitor::getPatternName(){
    return patternName;
}
void HardwareMonitor::setPatternName(QString patternName){
    this->patternName=patternName;
    emit updateValues();
}
int HardwareMonitor::getFreq(){
    return freq;
}
void HardwareMonitor::setFreq(int freq){
    this->freq=freq;
    emit updateValues();
}
int HardwareMonitor::getValue(){
    return value;
}
void HardwareMonitor::setValue(int value){
    this->value=value;
}
int HardwareMonitor::getExtraValue(){
    return extraValue;
}
void HardwareMonitor::setExtraValue(int extraValue){
    this->extraValue=extraValue;
}
int HardwareMonitor::getFreqCounter(){
    return freqCounter;
}
void HardwareMonitor::setFreqCounter(int freqCounter){
    this->freqCounter=freqCounter;
    emit updateValues();
}
QJsonObject HardwareMonitor::toJson()
{
    QJsonObject obj;
    obj.insert("name", name);
    obj.insert("type", type);
    obj.insert("lvl", lvl);
    obj.insert("action", action);
    obj.insert("patternName", patternName);
    obj.insert("freq", freq);
    if(status!="checking...")
        obj.insert("freqCounter", freqCounter);
    else
        obj.insert("freqCounter", 0);
    obj.insert("value", value);
    obj.insert("extraValue", extraValue);
    obj.insert("done", done);
    obj.insert("role", role);
    if(status!="checking...")
        obj.insert("status",status);
    else
        obj.insert("status",QString("NO VALUE"));
    return obj;
}
void HardwareMonitor::fromJson( QJsonObject obj)
{
    setName(obj.value("name").toString());
    setType(obj.value("type").toDouble());
    setLvl(obj.value("lvl").toDouble());
    setAction(obj.value("action").toDouble());
    setPatternName(obj.value("patternName").toString());
    setFreq(obj.value("freq").toDouble());
    setFreqCounter(obj.value("freqCounter").toDouble());
    setValue(obj.value("value").toDouble());
    setExtraValue(obj.value("extraValue").toDouble());
    done=obj.value("done").toBool();
    setRole(obj.value("role").toDouble());
    setStatus(obj.value("status").toString());
}
void HardwareMonitor::changeFreqCounter(){
    if(freq==0) freq=3;
    freqCounter=(freqCounter+1)%freq;
}
QString HardwareMonitor::getStatus(){
    return status;
}
bool HardwareMonitor::getDone(){
    return done;
}
int HardwareMonitor::getRole(){
    return role;
}
void HardwareMonitor::setRole(int role){
    this->role=role;
}
bool HardwareMonitor::checkValue(int value){
    bool result=false;
    if(action==0){
        if(value<lvl)
            result=true;
    }else if(action==1){
        if(value<=lvl)
            result=true;
    }else if(action==2){
        if(value==lvl)
            result=true;
    }else if(action==3){
        if(value>lvl)
            result=true;
    }else if(action==4){
        if(value>=lvl)
            result=true;
    }
    return result;
}
void HardwareMonitor::setStatus(QString status){
    this->status=status;
}
HardwareMonitor::~HardwareMonitor(){
    if(p!=NULL){
        p->close();
        disconnect(p);
        delete p;
    }
    if(p2!=NULL){
        p2->close();
        disconnect(p2);
        delete p2;
    }
    if(p3!=NULL){
        p3->close();
        disconnect(p3);
        delete p3;
    }
}
void HardwareMonitor::setDone(bool done){
    this->done=done;
}
void HardwareMonitor::setEditing(bool e){
    if(p!=NULL){
        p->close();
        disconnect(p);
        delete p;
    }
    if(p2!=NULL){
        p2->close();
        disconnect(p2);
        delete p2;
    }
    if(p3!=NULL){
        p3->close();
        disconnect(p3);
        delete p3;
    }
    this->editing=e;
    this->status="In edit mode";
    this->done=false;
    emit updateOnlyStatusAndCurrentValue();
}
