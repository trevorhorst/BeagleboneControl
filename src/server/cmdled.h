#ifndef CMDLED_H
#define CMDLED_H

#include <QObject>

class cmdled : public QObject
{
    Q_OBJECT
public:
    explicit cmdled(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CMDLED_H