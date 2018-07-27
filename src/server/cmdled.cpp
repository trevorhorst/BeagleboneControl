#include "cmdled.h"

static QString PARAM_BLINK    = "blink";
static QString PARAM_ENABLE	  = "enable";
static QString PARAM_ID		  = "id";
static QString PARAM_INTERVAL = "interval";

CmdLed::CmdLed( Led* cntrlObj, QObject *parent )
    : Command( parent )
    , mControlObject( cntrlObj )
{
    mParameterFunctions.insert( PARAM_BLINK, &CmdLed::Blink );
    mParameterFunctions.insert( PARAM_ENABLE, &CmdLed::Enable );
    mParameterFunctions.insert( PARAM_INTERVAL, &CmdLed::Interval );
}

bool CmdLed::Set( QVariantMap parameterList)
{
    printf( "%s : %s\n", __FILE__,  __FUNCTION__ );

    uint8_t id = 0;

    // Check for the required parameter
    QVariant val = parameterList.value( PARAM_ID, QVariant() );
    if( val.isNull() ) {
        qDebug( "CmdLed: Required %s parameter NOT found"
                , qPrintable( PARAM_ID ) );
        return false;
    } else {
        id = val.toUInt();
        parameterList.remove( PARAM_ID );
    }

    // Grab the parameter list
    QList< QString > parameters = parameterList.keys();
    for( int i = 0; i < parameters.size(); i++ ) {
        // Check if we have a function for the parameter
        ParameterFunction func = mParameterFunctions.value(
                    parameters.at( i ), NULL );
        if( func == NULL ) {
            qDebug( "CmdLed: %s parameter NOT found", qPrintable( parameters.at( i ) ) );
            return false;
        } else {
            bool rval = ( this->*func )( id, parameterList );
        }
    }
    return true;
}

bool CmdLed::Query()
{
    printf( "%s : %s\n", __FILE__,  __FUNCTION__ );

    return true;
}

bool CmdLed::Blink( uint8_t id, const QVariantMap &params )
{
    printf( "%s\n", __FUNCTION__ );
    bool enable = params.value( PARAM_BLINK ).toBool();
    mControlObject->SetBlink( enable );
    return true;
}

bool CmdLed::Enable( uint8_t id, const QVariantMap &params )
{
    printf( "%s\n", __FUNCTION__ );
    bool enable = params.value( PARAM_ENABLE ).toBool();
    mControlObject->Enable( enable );
    return true;
}

bool CmdLed::Interval(uint8_t id, const QVariantMap &params)
{
    printf( "%s\n", __FUNCTION__ );
    uint16_t interval = params.value( PARAM_INTERVAL ).toUInt();
    return mControlObject->SetInterval( interval );
}
