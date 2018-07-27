#ifndef CONSOLE_H
#define CONSOLE_H

#include <QThread>
#include <QObject>
#include <unistd.h>

#include "script.h"
#include <readline/readline.h>
#include <readline/history.h>

class Console : public QObject
{
    Q_OBJECT
public:
    Console();
    ~Console();

    void Init( Script *script );

signals:
    void Done();
    void Exited();

public slots:
    void Start();
    void Run();
    void Exit();
    void Test(){ qDebug("Test"); }

private:
    Script *mScript;
    QThread mThread;

    bool mDone;
};

#endif // CONSOLE_H
