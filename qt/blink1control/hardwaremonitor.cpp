#include "hardwaremonitor.h"

#include <QRegularExpression>


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

void HardwareMonitor::checkMonitor()
{
    if(editing) {
        return;
    }
    status="checking...";
    emit updateOnlyStatusAndCurrentValue();
    if(      type==0 ) {  checkBattery(); }
    else if( type==1 ) {  checkCpu(); } 
    else if( type==2 ) {  checkRam(); }
}

void HardwareMonitor::checkBattery()
{
#ifdef Q_OS_MAC
    if(p3!=NULL) return;
    p3=new QProcess();
    p3->start("pmset -g batt");
    connect(p3,SIGNAL(finished(int)),this,SLOT(readyReadBattery(int)));
#endif
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
        if(checkValue(value) ) { // && !done){
            if(role>0){
                emit addReceiveEvent(0,QString::number(value)+"% Battery",name);
                emit runPattern(patternName,false);
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
}

void HardwareMonitor::readyReadBattery(int exitCode)
{
    if( exitCode != 0 ) {  // all is well
        status = "Internal Error";
    }
    else {
        QString p3out = p3->readAllStandardOutput();
        if( p3out.contains("InternalBattery") ) { 
            QRegularExpression   re("InternalBattery.+\\s(\\d+)%");
            QRegularExpressionMatch match = re.match(p3out);
            if( match.hasMatch() ) {
                QString percentstr = match.captured( match.lastCapturedIndex() );
                value = percentstr.toInt();
                status = percentstr +"%";
                if(checkValue(value) ) { // && !done ) {
                    if(role>0){  // FIXME: why role>0 ?
                        status += "!";
                        emit addReceiveEvent(0,QString::number(value)+"% Battery",name);
                        emit runPattern(patternName,false);
                    }
                    done=true;
                }
            }
            else { 
                status = "Bad Battery Read";
            }
        }
        else { 
            status = "No Battery";
        }
    }
    qDebug() << "readyReadBattery: "<<status;
    emit updateOnlyStatusAndCurrentValue();
    p3->close();
    disconnect(p3);
    p3=NULL;
}

void HardwareMonitor::checkCpu()
{
#ifdef Q_OS_WIN
    if(p!=NULL) return;
    p=new QProcess();
    p->start("wmic cpu get loadpercentage");
    connect(p,SIGNAL(finished(int)),this,SLOT(readyReadCpu(int)));
#endif
#ifdef Q_OS_MAC
    if(p!=NULL) return;
    p=new QProcess();
    p->start("sh -c \"top -l 1 -s 0 | grep 'CPU usage:'\"");  //"top -o cpu -l 1");
    connect(p,SIGNAL(finished(int)),this,SLOT(readyReadCpu(int)));
#endif
}

void HardwareMonitor::readyReadCpu(int exitCode)
{
    bool correct=false;
#ifdef Q_OS_WIN
    if(exitCode!=0) {  // bad exit code
        status = "Internal Proc Error";
    }
    else { 
        QString pout = p->readAllStandardOutput();
        QRegularExpression re("LoadPercentage\\s+(\\d+)\\s");
        QRegularExpressionMatch match = re.match(pout);
        if( match.hasMatch() ) {
            QString percentstr = match.captured( match.lastCapturedIndex() );
            value = percentstr.toInt();
            correct = true;
        }
    }
#endif
#ifdef Q_OS_MAC
    if(exitCode!=0) {
        status = "Internal Proc Error";
    } 
    else { 
        QString pout = p->readAllStandardOutput();
        QRegularExpression re("CPU usage.*user,\\s+(\\d+).\\d+%\\s");
        QRegularExpressionMatch match = re.match(pout);
        if( match.hasMatch() ) {
            QString percentstr = match.captured( match.lastCapturedIndex() );
            value = percentstr.toDouble();
            correct = true;
        }
    }
#endif

    qDebug() << "readyReadCpu:"<<role<<": percent="<<value <<" done:"<<done << " correct:"<<correct;
    if( correct ){
        if(role==0 || role==2){
            status=QString::number(value)+"%";
        }else{
            status="Level not reached!";
        }
        if(checkValue(value) ) { // && !done){
            if(role>0){
                emit addReceiveEvent(0,QString::number(value)+"% CPU",name);
                emit runPattern(patternName,false);
            }
            if(role==1)
                status="Level reached!";
            done=true;
        }else{
            done=false;
        }
    }else{
        status="Internal error";
        done=false; // FIXME: what is "done" for again?
        freqCounter=0;
    }
 
    emit updateOnlyStatusAndCurrentValue();
    p->close();
    disconnect(p);
#ifdef Q_OS_WIN
    //delete p;  // FIXME: why was this here? causes crash on my box
#endif
    p=NULL;
}

void HardwareMonitor::checkRam()
{
#ifdef Q_OS_WIN
    if(p2!=NULL) return;
    p2=new QProcess();
    // FIXME: dont spawn processes for this
    p2->start("wmic OS get FreePhysicalMemory,TotalVisibleMemorySize");
    connect(p2,SIGNAL(finished(int)),this,SLOT(readyReadRam(int)));
#endif
#ifdef Q_OS_MAC
    if(p2!=NULL) return;
    p2=new QProcess();
    // FIXME: dont spawn processes for this
    p2->start("sh -c \"sysctl hw.memsize && top -o cpu -l 1 -s 0 | grep 'PhysMem:'\"");
    connect(p2,SIGNAL(finished(int)),this,SLOT(readyReadRam(int)));
#endif
}

void HardwareMonitor::readyReadRam(int exitCode)
{
    bool correct=false;
#ifdef Q_OS_WIN
    if(exitCode!=0) {
        status = "Internal Proc Error";
    } 
    else { 
        QString p2out = p2->readAllStandardOutput();
        //qDebug() << "readyReadRam:"<<p2out;
        QRegularExpression re("FreePhysicalMemory\\s+TotalVisibleMemorySize\\s+(\\d+)\\s+(\\d+)");
        QRegularExpressionMatch match = re.match(p2out);
        if( match.hasMatch() ) {
            QString freestr  = match.captured( 1 );  // memsize in bytes
            QString totalstr = match.captured( 2 );
            long free = freestr.toLong();
            long total =totalstr.toLong();
            value = 100 * (total -free)/total;
            correct = true;
        }
    }
#endif
#ifdef Q_OS_MAC
    if(exitCode!=0) {
        status = "Internal Proc Error";
    } 
    else { 
        QString p2out = p2->readAllStandardOutput();
        //qDebug() << "readyReadRam:"<<p2out;
        QRegularExpression re("hw.memsize: (\\d+)\\s+PhysMem:.+?(\\d+)([KMG]) used");
        QRegularExpressionMatch match = re.match(p2out);
        if( match.hasMatch() ) {
            QString totalstr = match.captured( 1 );  // memsize in bytes
            QString usedstr  = match.captured( 2 );
            QString mult     = match.captured( 3 );
            int total = totalstr.toLong() / 1024 / 1024;  // convert bytes to MBs
            int used  = usedstr.toInt();
            used = (mult=="G") ? used*1024 : (mult=="K") ? used/1024 : used;
            qDebug() << "totalstr:"<< totalstr <<", usedstr:"<< usedstr <<",mult:"<< mult
                     <<"total:"<<total<<", used:"<< used ;
            value = 100 * used / total;
            correct = true;
        }
        else { 
            qDebug() << "arggg";
        }
    }
#endif

    qDebug() << "readyReadRam:"<<role<<": percent="<<value <<" done:"<<done << " correct:"<<correct;
    if( correct ) {
        if(role==0 || role==2){
            status=QString::number(value)+"%";
        }else{
            status="Level not reached!";
        }
        if(checkValue(value) ) { //  && !done){
            if(role>0){
                emit addReceiveEvent(0,QString::number(value)+"% RAM",name);
                emit runPattern(patternName,false);
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
    qDebug() << "checkValue:"<<value;
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
