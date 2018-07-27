#ifndef CMDGPIO_H
#define CMDGPIO_H

#include "hwlib/gpio.h"

#include "server/command.h"

class CmdGpio : public Command
{
public:
    CmdGpio( GPIO* cntrlObj, QObject *parent = nullptr );
    bool Set( QVariantMap parameterList );
    bool Query();

signals:

public slots:

private:
    typedef bool (CmdGpio::*ParameterFunction)( uint8_t bank
                                                , uint8_t pin
                                                , const QVariantMap &params );

    bool Enable( uint8_t bank, uint8_t pin, const QVariantMap &params );
    bool Output( uint8_t bank, uint8_t pin, const QVariantMap &params );

    QHash< QString, ParameterFunction > mParameterFunctions;
    GPIO *mControlObject;

};

#endif // CMDGPIO_H
