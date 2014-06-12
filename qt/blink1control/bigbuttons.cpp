#include "bigbuttons.h"

BigButtons::BigButtons(QString name,QColor col)
{
    this->name=name;
    this->col=col;
    this->patternName="";
    led=0;
}
BigButtons::BigButtons(QString name,QString pn)
{
    this->name=name;
    this->patternName=pn;
    this->col=QColor("black");
    led=0;
}
QString BigButtons::getName(){
    return name;
}
QColor BigButtons::getCol(){
    return col;
}
void BigButtons::setColor(QColor col){
    this->col=col;
    emit updateValues();
}
void BigButtons::setName(QString name){
    this->name=name;
    emit updateValues();
}
QString BigButtons::getPatternName(){
    return patternName;
}
void BigButtons::setPatternName(QString pn){
    this->patternName=pn;
    emit updateValues();
}
QJsonObject BigButtons::toJson()
{
    QJsonObject obj;
    obj.insert("name", name);
    obj.insert("col",col.name());
    obj.insert("patternName",patternName);
    obj.insert("led",getLed());
    return obj;
}
void BigButtons::fromJson( QJsonObject obj)
{
    setName(obj.value("name").toString());
    setColor(QColor(obj.value("col").toString()));
    setPatternName(obj.value("patternName").toString());
    setLed(obj.value("led").toDouble());
}
int BigButtons::getLed(){
    return led;
}
void BigButtons::setLed(int l){
    led=l;
}
