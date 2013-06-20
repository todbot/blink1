#ifndef BLINK1INPUT_H
#define BLINK1INPUT_H

#include <QObject>
#include <QJsonObject>

class Blink1Input : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString type READ type WRITE setType)
    Q_PROPERTY(QString arg1 READ arg1 WRITE setArg1)
    //Q_PROPERTY(QString arg2 READ arg2 WRITE setArg2)

public:
    explicit Blink1Input(QObject *parent = 0);
    
    QJsonObject toJson();
    void fromJson( QJsonObject obj);  // make class method?

    QString name() const;
    void setName(const QString& name);
    QString type() const;
    void setType(const QString& type);
    QString arg1() const;
    void setArg1(const QString& arg1);
    //QString arg2() const;
    //void setArg2(const QString& arg2);

signals:
    
public slots:

private:
    QString mname;
    QString mtype;
    QString marg1;
    QString marg2;

};

#endif // BLINK1INPUT_H
