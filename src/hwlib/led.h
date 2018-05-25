#ifndef LED_H
#define LED_H

#include <QObject>

class led : public QObject
{
    Q_OBJECT
public:
    explicit led(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LED_H