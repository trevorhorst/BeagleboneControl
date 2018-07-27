#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>

#include "server/cmdgpio.h"
#include "server/cmdled.h"

class CommandHandler : public QObject
{
    Q_OBJECT
public:
    explicit CommandHandler(QObject *parent = nullptr);

    bool AddCommand( QString type, Command *cmd );

    QHash< QString, Command* > mCommandList;
signals:

public slots:

private:
};

#endif // COMMANDHANDLER_H
