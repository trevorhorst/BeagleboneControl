#include "led.h"

bool Led::mVerbose = false;
QTimer Led::mTimer;
uint32_t Led::mCount = 0;

uint32_t Led::GetCount()
{
    return mCount;
}

Led::Led( uint8_t bank, uint8_t pin, QObject *parent )
    : QObject( parent )
    , mGpio( nullptr )
    , mBank( bank )
    , mPin( pin )
    , mId( mCount++ )
{
    // connect( &mTimer, SIGNAL( timeout() ), this, SLOT( Toggle() ) );
    if( !mTimer.isActive() ) {
        mTimer.setInterval( 1000 );
        mTimer.start();
    }
}

Led::~Led()
{
    if( mTimer.isActive() ) {
        mTimer.stop();
    }
}

void Led::Init( GPIO *gpio )
{
    mGpio = gpio;
}

void Led::Enable( bool enable )
{
    if( mGpio == nullptr ){ return; }

    // Toggle the direction to output, just in case
    mGpio->Dir( mBank, mPin, true );

    if( enable ) {
        mGpio->Set( mBank, mPin );
    } else {
        mGpio->Clr( mBank, mPin );
    }
}

void Led::Toggle()
{
    static bool toggle = false;
    Enable( toggle );
    toggle = !toggle;
}

void Led::SetBlink( bool enable )
{
    if( enable ) {
        connect( &mTimer, SIGNAL( timeout() ), this, SLOT( Toggle() ) );
    } else {
        disconnect( &mTimer, SIGNAL( timeout() ), this, SLOT( Toggle() ) );
    }
}

bool Led::SetInterval( quint16 interval )
{
    if( interval < 250 || interval > 1000 ) {
        return false;
    }
    mTimer.setInterval( interval );
    return true;
}
