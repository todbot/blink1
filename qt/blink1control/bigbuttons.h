#ifndef BIGBUTTONS_H
#define BIGBUTTONS_H
#include <QObject>
#include <QColor>
#include <QJsonObject>
class BigButtons: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY updateValues)
    Q_PROPERTY(QColor col READ getCol WRITE setColor NOTIFY updateValues)
    Q_PROPERTY(QString patternName READ getPatternName WRITE setPatternName NOTIFY updateValues)
public:
    BigButtons(QString,QColor);
    BigButtons(QString,QString);
    QString getName();
    QColor getCol();
    void setColor(QColor col);
    void setName(QString name);
    QString getPatternName();
    void setPatternName(QString pn);
    QJsonObject toJson();
    void fromJson( QJsonObject obj);
    int getLed();
    void setLed(int l);

signals:
    void updateValues();
private:
    QString name;
    QColor col;
    QString patternName;
    int led;
};

#endif // BIGBUTTONS_H
