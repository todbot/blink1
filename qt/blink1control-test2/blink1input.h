#ifndef BLINK1INPUT_H
#define BLINK1INPUT_H

#include <QObject>
#include <QJsonObject>

class Blink1Input : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)

public:
    explicit Blink1Input(QObject *parent = 0);
    
    QJsonObject toJson();
    void fromJson( QJsonObject obj);  // make class method?

    QString name() const;
    void setName(const QString& name);

signals:
    
public slots:

private:
    QString mname;


};

#endif // BLINK1INPUT_H
