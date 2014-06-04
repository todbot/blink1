
#include "blink1pattern.h"

#include <QDebug>

Blink1Pattern::Blink1Pattern( QObject *parent) : QObject(parent)
{
    resetObj();
    setName("");
    mdate=QDateTime::currentDateTime().toTime_t();
    t=NULL;
    mreadonly=false;
    msystem=false;
}

Blink1Pattern::Blink1Pattern(const char* name, QObject *parent) : QObject(parent)
{
    setName(name);
    mdate=QDateTime::currentDateTime().toTime_t();
    t=NULL;
}

void Blink1Pattern::fromJson( QJsonObject obj)
{
    setName( obj.value("name").toString() );
    setRepeats(obj.value("repeats").toInt());
    QString tmp=obj.value("pattern").toString();
    fromPatternStrWithLeds(tmp);
    setDate(obj.value("date").toDouble());
    setReadOnly(obj.value("readonly").toBool());
    setSystem(obj.value("system").toBool());
}

QJsonObject Blink1Pattern::toFullJsonReadyToSave()
{
    QJsonObject obj;
    obj.insert("name", name());
    obj.insert("repeats", repeats());
    obj.insert("pattern", patternStrWithLeds());
    obj.insert("date",date());
    obj.insert("readonly",isReadOnly());
    obj.insert("system",isReadOnly());
    return obj;
}
QJsonObject Blink1Pattern::toJsonWithNameAndPatternStr()
{
    QJsonObject obj;
    obj.insert("name", name());
    obj.insert("pattern", patternStr());
    return obj;
}
void Blink1Pattern::resetObj()
{
    mname = "";
    mrepeats = 0;
    mplaycount = 0;
    mplaying = 0;
    mplaypos = 0;
    colors = QList<QColor>();
    times = QList<float>();
    mreadonly=false;
    msystem=false;
}

QString Blink1Pattern::patternStr()
{
    QString str = QString("%1").arg( mrepeats );
    for( int i=0; i<colors.count(); i++) {
        str.append( QString(",%1,%2").arg(colors.at(i).name().replace("#","%23")).arg(times.at(i)));
    }
    return str;
}

QString Blink1Pattern::patternStrWithLeds()
{
    QString str = QString("%1").arg( mrepeats );
    for( int i=0; i<colors.count(); i++) {
        str.append( QString(",%1,%2,%3").arg(colors.at(i).name()).arg(times.at(i)).arg(leds.at(i)));
    }
    return str;
}

// parse pattern string "repeats,color1,color1time,color2,color2time,..."
void Blink1Pattern::fromPatternStr(QString tmp){
    tmp.replace(" ",""); // remove any whitepsace from mucking up parsing
    QStringList list=tmp.split(",");
    // minimal pattern string is "reps,color,time" & pair count must be even
    if( list.count() > 3 && (list.count()-1) % 2 == 0 ) { 
        setRepeats(list.at(0).toInt());
        for(int i=1;i<list.count();i+=2){
            addColorAndTime(list.at(i),list.at(i+1).toDouble());
        }
    }
}
// parse pattern string "repeats,color1,color1time,led1,color2,color2time,led2,..."
void Blink1Pattern::fromPatternStrWithLeds(QString tmp){
    tmp.replace(" ",""); // remove any whitepsace from mucking up parsing
    QStringList list=tmp.split(",");
    setRepeats(list.at(0).toInt());
    if((list.count()>=4 && list.at(3).indexOf(QRegExp("#([0-9a-fA-F]{6})"))!=-1) || list.count()==3){
        fromPatternStr(tmp);
        return;
    }
    for(int i=1;i<list.count();i+=3){
        addColorAndTime(list.at(i),list.at(i+1).toDouble());
        editLed(colors.count()-1,list.at(i+2).toInt());
    }
}

QString Blink1Pattern::name() const
{
    return mname;
}

void Blink1Pattern::setName(const QString &name)
{
    mname = name;
    emit updateValues();
}

void Blink1Pattern::update()
{
    if(!mplaying) return;
    t->stop();
    delete t;
    t=NULL;

    currentColor++;
    if(currentColor<count){
        startR+=deltaR;
        startG+=deltaG;
        startB+=deltaB;
        t=new QTimer(this);
        t->setInterval(50);
        t->setSingleShot(true);
        connect(t,SIGNAL(timeout()),this,SLOT(update()));
        t->start();
        emit changeColorOnVirtualBlink(QColor(startR,startG,startB));
    }else{
        currentColor=0;
        mplaypos++;
        if(mplaypos==colors.count()){
            mplaypos=0;
            mplaycount++;
        }
        count=activeTime()*1000/50;
        currentColor=0;
        startR+=deltaR;
        startG+=deltaG;
        startB+=deltaB;
        deltaR=(activeColor().red()-startR)*1.0/count;
        deltaG=(activeColor().green()-startG)*1.0/count;
        deltaB=(activeColor().blue()-startB)*1.0/count;

        t=new QTimer(this);
        t->setInterval(50);
        t->setSingleShot(true);
        connect(t,SIGNAL(timeout()),this,SLOT(update()));
        t->start();
        emit changeColorOnVirtualBlink(QColor(startR,startG,startB));

        if(mplaycount==mrepeats+(mrepeats!=-1)?1:0){
            stop();
            return;
        }
        emit setColor(activeColor(),mname,activeTime()*1000); // convert to millis
    }
}

void Blink1Pattern::play(QColor currentVirtualBlinkColor)
{
    if(mplaying) return;
    mplaycount=0;
    mplaypos=0;
    mplaying=true;
    emit updatePlayIconOnUi();
    t=new QTimer(this);

    count=activeTime()*1000/50;
    currentColor=0;
    startR=currentVirtualBlinkColor.red();
    startG=currentVirtualBlinkColor.green();
    startB=currentVirtualBlinkColor.blue();
    deltaR=(activeColor().red()-currentVirtualBlinkColor.red())*1.0/count;
    deltaG=(activeColor().green()-currentVirtualBlinkColor.green())*1.0/count;
    deltaB=(activeColor().blue()-currentVirtualBlinkColor.blue())*1.0/count;


    t->setInterval(50);
    t->setSingleShot(true);

    connect(t,SIGNAL(timeout()),this,SLOT(update()));
    t->start();

    emit changeColorOnVirtualBlink(QColor(startR,startG,startB));
    emit setColor(activeColor(),mname,activeTime()*1000); // convert to millis
}

void Blink1Pattern::stop()
{
    mplaycount=0;
    mplaypos=0;
    mplaying=false;
    emit updatePlayIconOnUi();
    emit setColor(QColor("#000000"),"",0);
    if(t!=NULL){
        t->stop();
        delete t;
        t=NULL;
    }
}

int Blink1Pattern::repeats()
{
    return mrepeats;
}
int Blink1Pattern::playcount()
{
    return mplaycount;
}


void Blink1Pattern::setRepeats(int r)
{
    mrepeats = r;
    emit updateValues();
}
void Blink1Pattern::setPlaycount(int p)
{
    mplaycount = p;
    emit updateValues();
}
void Blink1Pattern::setColors(QList<QColor> lc){
    colors=lc;
    emit updateValues();
}
void Blink1Pattern::setTimes(QList<float> lt){
    times=lt;
    emit updateValues();
}
QColor Blink1Pattern::activeColor(){
    return colors.at(mplaypos);//mplaycount%colors.count());
}
float Blink1Pattern::activeTime(){
    return times.at(mplaypos);//mplaycount%times.count());
}
bool Blink1Pattern::playing(){
    return mplaying;
}
QList<QColor> Blink1Pattern::getColorList(){
    return colors;
}
void Blink1Pattern::clearArrays(){
    colors.clear();
    times.clear();
    leds.clear();
}
QVariantList Blink1Pattern::getColors(){
    QVariantList v;
    for(int i=0;i<colors.count();i++){
        v.append(QVariant(colors.at(i).name()));
    }
    return v;
}
QVariantList Blink1Pattern::getTimes(){
    QVariantList v;
    for(int i=0;i<colors.count();i++){
        v.append(QVariant(times.at(i)));
    }
    return v;
}
QVariantList Blink1Pattern::getLeds(){
    QVariantList v;
    for(int i=0;i<leds.count();i++){
        v.append(QVariant(leds.at(i)));
    }
    return v;
}

void Blink1Pattern::addColorAndTime(QString color, double time){
    colors.append(QString(color));
    times.append(time);
    leds.append(0);
    emit updateValues();
}
void Blink1Pattern::removeColorAndTime(int idx){
    if(colors.count()==1) return;
    colors.removeAt(idx);
    times.removeAt(idx);
    leds.removeAt(idx);
    emit updateValues();
}
void Blink1Pattern::changeRepeats(){
    mrepeats=mrepeats+1;
    if(mrepeats==5) mrepeats=-1;
    emit updateValues();
}

void Blink1Pattern::editColorAndTime(QString color, double time, int idx) {
    colors[idx] = QColor(color);
    times[idx] = time;
    emit updateValues();
}
void Blink1Pattern::editLed(int idx, int led){
    leds[idx]=led;
    emit updateValues();
}
int Blink1Pattern::getLed(int idx){
    return leds[idx];
}

int Blink1Pattern::date(){
    return mdate;
}
void Blink1Pattern::setDate(int date){
    this->mdate=date;
}
void Blink1Pattern::setReadOnly(bool ro){
    this->mreadonly=ro;
    emit updateValues();
}
bool Blink1Pattern::isReadOnly(){
    return mreadonly;
}
void Blink1Pattern::setSystem(bool sy){
    this->msystem=sy;
    emit updateValues();
}
bool Blink1Pattern::isSystem(){
    return msystem;
}
int Blink1Pattern::isPlaying(){
    return (mplaying)?1:0;
}
QColor Blink1Pattern::getColor(int idx){
    return colors[idx];
}
double Blink1Pattern::getTime(int idx){
    return times[idx];
}
int Blink1Pattern::getCurrentLed(){
    return leds.at(mplaypos);
}
