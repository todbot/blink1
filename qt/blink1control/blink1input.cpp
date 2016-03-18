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
    setType(obj.value("type").toString().toLower());
    setArg1(obj.value("arg1").toString());
    setArg2(obj.value("arg2").toString());
    setDate(obj.value("date").toString().toLongLong());
    setFreq(obj.value("freq").toDouble());
    setFreqCounter(obj.value("freqCounter").toDouble());
    setPatternName(obj.value("patternName").toString());
}

QJsonObject Blink1Input::toJson() // omg, was "toFullJsonReadyToSave()"
{
    //qDebug() << "blink1input::toJson: date:"<< date() << " datestr:"<< QString::number( date() );
    QJsonObject obj;
    obj.insert("name", name());
    obj.insert("type",type());
    obj.insert("arg1",arg1());
    obj.insert("arg2",arg2());
    obj.insert("date", QString::number( date() ) );
    obj.insert("patternName",patternName());
    obj.insert("freq",freq());
    obj.insert("freqCounter",freqCounter());
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
qint64 Blink1Input::date(){
    return mdate;
}
void Blink1Input::setDate(qint64 date){
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
