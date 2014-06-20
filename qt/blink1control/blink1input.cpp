#include "blink1input.h"

Blink1Input::Blink1Input(QObject *parent) :
    QObject(parent)
{
    mpause=false;
    mdate=-1;
    mfreq=3;
    mfreqCounter=0;
    toDelete=false;
    isChecking=false;
}


void Blink1Input::fromJson( QJsonObject obj)
{
    setName( obj.value("name").toString() );
    setType(obj.value("type").toString());
    setArg1(obj.value("arg1").toString());
    setArg2(obj.value("arg2").toString());
    setDate(obj.value("date").toDouble());
    setFreq(obj.value("freq").toDouble());
    setFreqCounter(obj.value("freqCounter").toDouble());
    setPatternName(obj.value("patternName").toString());
}

QJsonObject Blink1Input::toFullJsonReadyToSave()
{
    QJsonObject obj;
    obj.insert("name", name());
    obj.insert("type",type());
    obj.insert("arg1",arg1());
    obj.insert("arg2",arg2());
    obj.insert("date",date());
    obj.insert("patternName",patternName());
    obj.insert("freq",freq());
    obj.insert("freqCounter",freqCounter());
    return obj;
}
QJsonObject Blink1Input::toJsonWithNameTypePNameArg1Arg2AndDate()
{
    QJsonObject obj;
    obj.insert("arg1",arg1());
    obj.insert("iname", name());    
    if(mtype!="IFTTT.COM"){
        obj.insert("lastTime",date());
        obj.insert("lastVal",arg2());
    }else{
        QJsonArray ja;
        ja.append(QJsonValue(arg2()));
        obj.insert("possibleVals",ja);
    }

    obj.insert("pname",patternName());
    obj.insert("type",lowerType());
    return obj;
}
QJsonObject Blink1Input::toJsonWithNameTypeAndArg1()
{
    QJsonObject obj;
    obj.insert("iname", name());
    obj.insert("type",lowerType());
    if(mtype!="IFTTT.COM") obj.insert("arg1",arg1());
    return obj;
}

QString Blink1Input::name() const
{
    return mname;
}
void Blink1Input::setName(const QString &name)
{
    mname = name;
    emit updateValues();
}

QString Blink1Input::type() const
{
    return mtype;
}
QString Blink1Input::lowerType() const
{
    if(mtype=="IFTTT.COM")
        return "ifttt";
    else
        return QString(mtype).toLower();
}
void Blink1Input::setType(const QString &type)
{
    mtype = type;
    emit updateValues();
}

QString Blink1Input::arg1() const
{
    return marg1;
}
void Blink1Input::setArg1(const QString &arg1)
{
    marg1 = arg1;
    emit updateValues();
}
QString Blink1Input::arg2() const
{
    return marg2;
}
void Blink1Input::setArg2(const QString &arg2)
{
    marg2 = arg2;
    emit updateValues();
}
QString Blink1Input::patternName() const{
    return mpatternName;
}

void Blink1Input::setPatternName(const QString &patternName){
    mpatternName=patternName;
    emit updateValues();
}
bool Blink1Input::pause(){
    return mpause;
}
void Blink1Input::setPause(bool v){
    mpause=v;
}
int Blink1Input::date(){
    return mdate;
}
void Blink1Input::setDate(int date){
    mdate=date;
    emit updateTimeValue();
}
QString Blink1Input::getTime(){
    if(mdate==-1) return "0 secs ago";
    QDateTime tt=QDateTime::fromTime_t(mdate);
    QDateTime curr=QDateTime::currentDateTime();
    int days=tt.daysTo(curr);
    if(days>=1){
        return QString::number(days)+" days ago";
    }
    int hours=tt.secsTo(curr)/3600;
    if(hours>=1){
        return QString::number(hours)+" hours ago";
    }
    int min=tt.secsTo(curr)/60;
    int sec=tt.secsTo(curr)-min*60;
    if(min>=1)
        return QString::number(min)+" minutes ago";
    if(sec>=1)
        return QString::number(sec)+" secs ago";
    return "0 secs ago";
}
void Blink1Input::setFreq(int freq){
    mfreq=freq;
    emit updateValues();
}
int Blink1Input::freq(){
    return mfreq;
}
void Blink1Input::setFreqCounter(int freqC){
    mfreqCounter=freqC;
    emit updateValues();
}
int Blink1Input::freqCounter(){
    return mfreqCounter;
}
void Blink1Input::changeFreqCounter(){
    if(mfreq==0) mfreq=3;
    mfreqCounter=(mfreqCounter+1)%mfreq;
}
void Blink1Input::updateTime(){
    emit updateTimeValue();
}
