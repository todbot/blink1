#include "blink1input.h"

Blink1Input::Blink1Input(QObject *parent) :
    QObject(parent)
{
}


void Blink1Input::fromJson( QJsonObject obj)
{
    setName( obj.value("name").toString() );
    //setPlaycount( obj.value("playcount").toDouble());
}

QJsonObject Blink1Input::toJson()
{
    QJsonObject obj;
    obj.insert("name", name());
    return obj;
}


QString Blink1Input::name() const
{
    return mname;
}
void Blink1Input::setName(const QString &name)
{
    mname = name;
}

QString Blink1Input::type() const
{
    return mtype;
}
void Blink1Input::setType(const QString &type)
{
    mtype = type;
}

QString Blink1Input::arg1() const
{
    return marg1;
}
void Blink1Input::setArg1(const QString &arg1)
{
    marg1 = arg1;
}
