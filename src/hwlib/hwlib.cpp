#include "hwlib.h"

bool HwLib::mVerbose = false;

HwLib::HwLib( QObject *parent )
    : QObject( parent )

    //, mGps( "/dev/ttyO1", QSerialPort::Baud9600 )
{
    // Map the main memory
    mFd = open( "/dev/mem", O_RDWR | O_SYNC, ( mode_t ) 0600 );
    if( mFd == -1 ) {
        printf( "Error opening file: %s\n", strerror( errno ) );
        printf( "Need to run as root or simulated mode\n" );
        exit( -1 );
    }

    // Initialize the GPIO module
    mGpio.Init( mFd, false );

    // Initialize the clock module
    mClockModule.Init( mFd, false );

    // Initialize the LEDs
    mLeds.push_back( new Led( 1, 21 ) );
    mLeds.push_back( new Led( 1, 22 ) );
    mLeds.push_back( new Led( 1, 23 ) );
    mLeds.push_back( new Led( 1, 24 ) );

    // Write to the screen
    mOled.WriteCenter( "Hello World", 5 );
    mOled.WriteScreen();


    for( size_t i = 0; i < mLeds.size(); i++ ) {
        mLeds.at( i )->Init( &mGpio );
    }

    mHeartbeat.setInterval( 1000 );
    mHeartbeat.connect( &mHeartbeat, SIGNAL( timeout() )
                        , this, SLOT( Heartbeat() ) );
    mHeartbeat.connect( this, SIGNAL( Startheart() )
                        , &mHeartbeat, SLOT( start() ) );
    mHeartbeat.connect( this, SIGNAL( Stopheart() )
                        , &mHeartbeat, SLOT( stop() ) );
    mHeartbeat.start();
}

HwLib::~HwLib()
{
    close( mFd );
    mHeartbeat.stop();
    mHeartbeat.disconnect( &mHeartbeat, SIGNAL( timeout() )
                        , this, SLOT( Heartbeat() ) );
}

void HwLib::ToggleHeart()
{
    if( mHeartbeat.isActive() ) {
        emit Stopheart();
        qDebug( "Stopping Heartbeat" );
    } else {
        emit Startheart();
        qDebug( "Starting Heartbeat" );
    }
}

void HwLib::Heartbeat()
{
    // QGeoPositionInfo loc = mGps.GetLocation();
    // QString date = QString( "%0" ).arg(
    //             loc.timestamp().date().toString() );
    // QString time = QString( "%0" ).arg(
    //             loc.timestamp().time().toString() );
    // QString lon = QString( "%0" ).arg(
    //             loc.coordinate().longitude() );
    // QString lat = QString( "%0" ).arg(
    //             loc.coordinate().latitude() );

    // mOled.WriteCenter( date, 0 );
    // mOled.WriteCenter( time, 1 );

    // mOled.WriteCenter( "Latitude:", 3 );
    // mOled.WriteCenter( lat, 4 );

    // mOled.WriteCenter( "Longitude:", 6 );
    // mOled.WriteCenter( lon, 7 );

    // mOled.writeScreen();

}

GPIO* HwLib::GetGpio()
{
    return &mGpio;
}

ClockModule* HwLib::GetClockModule()
{
    return &mClockModule;
}

Led* HwLib::GetLed( uint32_t led )
{
    if( led <= mLeds.size() ) {
        return mLeds.at( led );
    }
    return nullptr;
}
