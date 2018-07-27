#include "console.h"

Console::Console()
    : QObject( NULL )
    , mScript( NULL )
    , mThread()
    , mDone( false )
{
}

Console::~Console()
{
    mThread.quit();
    mThread.wait( 1000 );
}

void Console::Init(Script *script)
{
    if( script != NULL ){
        mScript = script;
        mScript->AddGlobal( "console", this );
        // mScript->AddCommand( "help", "console.Help()", "Show command list" );
        mScript->AddCommand( "exit", "console.Exit()", "Exit the program" );
    }
    Start();
}

void Console::Start()
{
    // Attach signals
    connect( &mThread, SIGNAL( started() ), this, SLOT( Run() ) );

    // Start the thread
    moveToThread( &mThread );
    mThread.start();
}

void Console::Run()
{
    printf( "Build Time: %s %s\n", __DATE__, __TIME__ );
    // We need a valid script object if were going to run
    if( mScript == NULL ){
        qDebug( "Script object is NULL" );
        mDone = true;
    }

    while( !mDone ) {
        QString line = readline( "> " );
        if( line.trimmed().isEmpty() ) {
            continue;
        }

        mScript->Evaluate( line );

        add_history( qPrintable( line ) );

    }

    emit Done();
}

void Console::Exit()
{
    mDone = true;
}
