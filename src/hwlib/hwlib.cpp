#include "hwlib.h"

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

    mGpio.Init( mFd, false );

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

void HwLib::InitScript( Script *script )
{
    script->AddGlobal( "hwlib", this );
    script->AddCommand( "heartbeat", "hwlib.ToggleHeart()"
                        , "Toggle the heartbeat");

    mGpio.InitScript( script );
    mGps.InitScript( script );
    mOled.InitScript( script );
    // mSerial.InitScript( script );
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

void HwLib::InitDebug( Debug *debug )
{
    debug->Add( "gpio", &GPIO::IsVerbose, &GPIO::SetVerbose );
    debug->Add( "gps", &Venus634LPx::IsVerbose, &Venus634LPx::SetVerbose );
    debug->Add( "oled", &SSD1306::IsVerbose, &SSD1306::SetVerbose );
}

void HwLib::Heartbeat()
{
    QGeoPositionInfo loc = mGps.GetLocation();
    QString date = QString( "%0" ).arg(
                loc.timestamp().date().toString() );
    QString time = QString( "%0" ).arg(
                loc.timestamp().time().toString() );
    QString lon = QString( "%0" ).arg(
                loc.coordinate().longitude() );
    QString lat = QString( "%0" ).arg(
                loc.coordinate().latitude() );

    mOled.WriteCenter( date, 0 );
    mOled.WriteCenter( time, 1 );

    mOled.WriteCenter( "Latitude:", 3 );
    mOled.WriteCenter( lat, 4 );

    mOled.WriteCenter( "Longitude:", 6 );
    mOled.WriteCenter( lon, 7 );

    mOled.writeScreen();

}
