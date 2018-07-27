#include "cmdgpio.h"

static QString PARAM_BANK   = "bank";
static QString PARAM_PIN    = "pin";
static QString PARAM_ENABLE = "enable";
static QString PARAM_OUTPUT = "output";

CmdGpio::CmdGpio( GPIO *cntrlObj , QObject *parent )
    : Command( parent )
    , mControlObject( cntrlObj )
{
    mParameterFunctions.insert( PARAM_ENABLE, &CmdGpio::Enable );
    mParameterFunctions.insert( PARAM_OUTPUT, &CmdGpio::Output );
}

bool CmdGpio::Set( QVariantMap parameterList )
{
    printf( "CmdGpio::Set\n" );
    QList< QString > parameters = parameterList.keys();

    uint8_t bank = 0;
    uint8_t pin  = 0;

    // The bank parameter is required
    if( parameters.contains( PARAM_BANK ) ) {
        bank = parameterList.value( PARAM_BANK ).toUInt();
    } else {
        return false;
    }

    // The pin parameter is required
    if( parameters.contains( PARAM_PIN ) ) {
        pin = parameterList.value( PARAM_PIN ).toUInt();
    } else {
        return false;
    }

    for( int i = 0; i < parameters.size(); i++ ) {
        if( mParameterFunctions.contains( parameters.at( i ) ) ) {
            ParameterFunction func = mParameterFunctions.value( parameters.at( i ) );
            bool rval = ( this->*func )( bank, pin, parameterList );
        }

        // QString s = parameterList.value( parameters.at( i ) ).toString();
        // printf( "%s: %s\n", qPrintable( parameters.at( i ) ), qPrintable( s ) );
    }
    return true;
}

bool CmdGpio::Query()
{
    return true;
}

bool CmdGpio::Enable( uint8_t bank, uint8_t pin, const QVariantMap &params )
{
    qDebug( "%s", qPrintable( PARAM_ENABLE ) );
    bool enable = params.value( PARAM_ENABLE ).toBool();
    if( enable ) {
        mControlObject->Set( bank, pin );
    } else {
        mControlObject->Clr( bank, pin );
    }
    return true;
}

bool CmdGpio::Output( uint8_t bank, uint8_t pin, const QVariantMap &params )
{
    qDebug( "%s", qPrintable( PARAM_OUTPUT ) );
    bool output = params.value( PARAM_OUTPUT ).toBool();
    mControlObject->Dir( bank, pin, output );
    return true;
}
