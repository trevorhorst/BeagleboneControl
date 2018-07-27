#ifndef CMDLED_H
#define CMDLED_H

#include <QObject>
#include <QVariantMap>

#include "hwlib/led.h"

#include "server/command.h"

class CmdLed : public Command
{
public:
    explicit CmdLed( Led* cntrlObj, QObject *parent = nullptr );
    bool Set( QVariantMap parameterList );
    bool Query();

signals:

public slots:

private:
    typedef bool (CmdLed::*ParameterFunction)( uint8_t led
                                               , const QVariantMap &params );

    bool Enable( uint8_t id, const QVariantMap &params );
    bool Blink( uint8_t id, const QVariantMap &params );
    bool Interval( uint8_t id, const QVariantMap &params );

    QHash< QString, ParameterFunction > mParameterFunctions;
    Led *mControlObject;
};

#endif // CMDLED_H
