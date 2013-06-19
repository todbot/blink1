#ifndef BLINK1PATTERN_H
#define BLINK1PATTERN_H

#include <QObject>
#include <QList>
#include <QColor>
#include <QJsonObject>

class Blink1Pattern : public QObject
{
    Q_OBJECT

     Q_PROPERTY(QString name READ name WRITE setName)
     Q_PROPERTY(int repeats READ repeats WRITE setRepeats)
     Q_PROPERTY(int playcount READ playcount WRITE setPlaycount)


public:
    Blink1Pattern(QObject *parent=0);
    Blink1Pattern(const char* name, QObject *parent=0);

    QJsonObject toJson();
    void fromJson( QJsonObject obj);  // make class method?

    QString name() const;
    void setName(const QString& name);

    int repeats();
    void setRepeats(int r);
    int playcount();
    void setPlaycount(int p);

    void stop();
    void play();
    void update();

    void resetObj();

    QString patternStr();

private:
    QString mname;
    int mrepeats;
    int mplaycount;
    int mplaypos;
    bool mplaying;

    QList<QColor> colors;
    QList<float> times;

};

#endif // BLINK1PATTERN_H
