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
    this->freqCounter=0;
    this->done=false;
    this->role=0;
}
void HardwareMonitor::checkMonitor(){
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
        SYSTEM_POWER_STATUS status;
        GetSystemPowerStatus(&status);
        int life = status.BatteryLifePercent;
        int BTime = status.BatteryLifeTime;
        qDebug()<<status.ACLineStatus;  //1-charging 0-no charging
        qDebug()<<status.BatteryFlag;   //1- battery, 128-no battery

        if(status.BatteryFlag!=128) qDebug()<<life << "% and " << BTime << "Battery Life!\n";
        if(status.BatteryFlag==128){
            value=-1;
            extraValue=-5;
            done=false;
        }else{
            value=life;
            if(role==0 || role==2){
                extraValue=(BTime!=-1)?BTime:0;
            }else{
                extraValue=-3;
            }
            if(checkValue(value)){
                if(!done){
                    if(role>0){
                        emit addReceiveEvent(0,QString::number(value)+"% Battery",name);
                        emit runPattern(patternName,false);
                    }
                }
                if(role==1)
                    extraValue=-2;
                done=true;
            }else{
                done=false;
            }
        }
        emit update2();
    #endif
    #ifdef Q_OS_MAC

    #endif
}
void HardwareMonitor::checkCpu(){
    #ifdef Q_OS_WIN
        if(p!=NULL) return;
        p=new QProcess();
        p->start("wmic cpu get loadpercentage");
        connect(p,SIGNAL(finished(int)),this,SLOT(readyReadCpu(int)));
    #endif
}
void HardwareMonitor::readyReadCpu(int st){
    #ifdef Q_OS_WIN
        QString tmpp=p->readAllStandardOutput();
        tmpp=tmpp.simplified();
        QStringList pomm=tmpp.split(QRegExp("( +|\r\n)"));
        qDebug()<<pomm.at(1).toInt();
        qDebug()<<"Status: "<<st;
        value=pomm.at(1).toInt();
        if(st==0){
            extraValue=0;
            if(role==0 || role==2){
                extraValue=1;
            }else{
                extraValue=-3;
            }
            if(checkValue(value)){
                if(!done){
                    if(role>0){
                        emit addReceiveEvent(0,QString::number(value)+"% CPU",name);
                        emit runPattern(patternName,false);
                    }
                }
                if(role==1)
                    extraValue=-2;
                done=true;
            }else{
                done=false;
            }
        }else{
            extraValue=-5;
            done=false;
        }
        emit update2();
        p->close();
        disconnect(p);
        delete p;
        p=NULL;
    #endif
}

void HardwareMonitor::checkRam(){
    #ifdef Q_OS_WIN
        p2=new QProcess();
        p2->start("wmic OS get FreePhysicalMemory,TotalVisibleMemorySize");
        connect(p2,SIGNAL(finished(int)),this,SLOT(readyReadRam(int)));
    #endif
}
void HardwareMonitor::readyReadRam(int st){
    #ifdef Q_OS_WIN
        QString tmp=p2->readAllStandardOutput();
        QStringList pom=tmp.split(QRegExp("(  +|\r\n)"));
        qDebug()<<pom;
        double total=pom.at(4).toDouble()*1.0/(1024);
        double used=pom.at(3).toDouble()*1.0/(1024);
        qDebug()<<100.0-used*1.0/total*100<<"%";
        qDebug()<<st;
        value=100.0-used*1.0/total*100;
        if(st==0)
            extraValue=0;
        else
            extraValue=-1;
        p2->close();
        delete p2;
    #endif
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
    emit update();
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
    emit update();
}
int HardwareMonitor::getFreq(){
    return freq;
}
void HardwareMonitor::setFreq(int freq){
    this->freq=freq;
    emit update();
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
    emit update();
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
    obj.insert("freqCounter", freqCounter);
    obj.insert("value", value);
    obj.insert("extraValue", extraValue);
    obj.insert("done", done);
    obj.insert("role", role);
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
}
void HardwareMonitor::changeFreqCounter(){
    if(freq==0) freq=3;
    freqCounter=(freqCounter+1)%freq;
}
QString HardwareMonitor::getStatus(){
    if(type==0){
        if(extraValue>=-1){
            return QString::number(value)+"%";
        }else if(extraValue==-2){
            return "Level reached!";
        }else if(extraValue==-3){
            return "Level not reached.";
        }else{
            return "No battery!";
        }
    }else if(type==1){
        if(extraValue>=-1){
            return QString::number(value)+"%";
        }else if(extraValue==-2){
            return "Level reached!";
        }else if(extraValue==-3){
            return "Level not reached.";
        }else{
            return "Internal error!";
        }
    }
    return "";
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
