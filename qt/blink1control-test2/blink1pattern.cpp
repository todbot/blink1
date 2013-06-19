
#include "blink1pattern.h"

#include <QDebug>
#include <QJsonArray>

Blink1Pattern::Blink1Pattern( QObject *parent) : QObject(parent)
{
    qDebug() << "Created: "; // << QObject::name();
    resetObj();
}

Blink1Pattern::Blink1Pattern(const char* name, QObject *parent) : QObject(parent)
{
    resetObj();
    setName(name);
}

void Blink1Pattern::fromJson( QJsonObject obj)
{
    setName( obj.value("name").toString() );
    setPlaycount( obj.value("playcount").toDouble());
}

QJsonObject Blink1Pattern::toJson()
{
    QJsonObject obj;
    obj.insert("name", name());
    obj.insert("repeats", repeats());
    obj.insert("playcount", playcount());
    obj.insert("playpos", mplaypos);
    obj.insert("pattern", patternStr());
    return obj;
}

void Blink1Pattern::resetObj()
{
    //mname = 0;
    mrepeats = 0;
    mplaycount = 0;
    mplaying = 0;
    mplaypos = 0;
    colors = QList<QColor>();
    times = QList<float>();
    colors.append( QColor(33,77,99));
    times.append( 1.23 );
}

QString Blink1Pattern::patternStr()
{
    QString str = QString("%1").arg( mrepeats );
    for( int i=0; i<colors.count(); i++) {
        str.append( QString(",%1,%2").arg(colors.at(i).name()).arg(times.at(i)));
    }
    return str;
}

/*
QJsonArray carr;
QJsonArray tarr;
for( int i=0; i< colors.count(); i++ ) {
    carr.append( colors.at(i).name() );
    tarr.append( times.at(i) );
}
obj.insert("colors", carr);
obj.insert("times", tarr);
*/
/*
QByteArray Blink1Pattern::toJsonStr()
{
    QJsonDocument doc;
    return doc.setObject( toJson() );
}
*/
/*
 *void Blink1Pattern::fromJson(const QByteArray &jsonstr)
{
    // not implemented yet
}
*/

// Blink1Pattern::~Blink1Pattern()
 //{
 //   qDebug() << "Deleted: "; // << name();
 //}

//Blink1Pattern::Blink1Pattern(QString aname)
//{
    //name = aname;
//}


QString Blink1Pattern::name() const
{
    return mname;
}

void Blink1Pattern::setName(const QString &name)
{
    mname = name;
}

void Blink1Pattern::update()
{
    qDebug() << "update:";
}

void Blink1Pattern::play()
{
    qDebug() << "play:";
}

void Blink1Pattern::stop()
{
    qDebug() << "stop:";
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
}
void Blink1Pattern::setPlaycount(int p)
{
    mplaycount = p;
}
