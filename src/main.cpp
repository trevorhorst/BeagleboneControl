#include <QCoreApplication>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QHostAddress>
#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QList>

// #include <qhttpengine/filesystemhandler.h>
// #include <qhttpengine/handler.h>
// #include <qhttpengine/server.h>
// #include <qhttpengine/qobjecthandler.h>

#include "server/httpserver.h"
#include "server/cmdgpio.h"
#include "server/cmdled.h"

#include "client/httpclient.h"

#include "common/console.h"
#include "common/debug.h"
#include "common/script.h"

#include "hwlib/hwlib.h"

#define UNUSED( var ) do { (void)( expr ); } while( 0 )

int main(int argc, char *argv[])
{
    QCoreApplication a( argc, argv );

    // Build the command-line options
    QCommandLineParser parser;

    QCommandLineOption addressOption(
                QStringList() << "a" << "address"
                , "address to bind to"
                , "address"
                , "127.0.0.1"
    );
    parser.addOption( addressOption );

    QCommandLineOption portOption(
                QStringList() << "p" << "port"
                , "port to listen on"
                , "port"
                , "8000"
    );
    parser.addOption( portOption );

    QCommandLineOption interfaceOption(
                QStringList() << "i" << "interface"
                , "interface to use"
                , "interface"
                , "lo"
    );
    parser.addOption( interfaceOption );


    parser.addHelpOption();

    // Parse the options that were provided
    parser.process(a);

    parser.value( interfaceOption );

    printf( "Port option: %d\n", parser.value( portOption ).toUInt() );
    // Obtain the values
    // QHostAddress address = QHostAddress(parser.value(addressOption));
    // quint16 port = parser.value(portOption).toInt();
    // QString interface = parser.value( interfaceOption ).toHtmlEscaped();
    // QString address = "127.0.0.1";

    HwLib hwlib;

    // Initialize the server
    // NDR354Server *server = new NDR354Server( interface, address, port );
    HttpServer server;
    HttpClient client( &hwlib );
    client.SetUrl( parser.value( addressOption )
                   , parser.value( portOption ).toUInt() );


    // This will hold all of our available commands
    CommandHandler commandHandler;

    // Initalize the command object with what it will be controlling
    CmdGpio cmdGpio( hwlib.GetGpio() );
    CmdLed cmdLed( hwlib.GetLed( 0 ) );
    commandHandler.AddCommand( "gpio", &cmdGpio );
    commandHandler.AddCommand( "led", &cmdLed );
    server.SetCommandHandler( &commandHandler );


    // Initialize scripting
    Script script;
    script.AddGlobal( "script", &script );
    script.AddCommand( "help", "script.Help()", "Display list of available commands" );
    script.AddGlobal( "server", &server );
    script.AddGlobal( "client", &client );
    script.AddCommand( "cmd", "client.BuildCommand('%0',['%1',%2,'%3',%4,'%5',%6])", "Build client command" );
    script.AddGlobal( "hwlib", &hwlib );
    script.AddGlobal( "gpio", hwlib.GetGpio() );
    script.AddGlobal( "clockmodule", hwlib.GetClockModule() );
    script.AddGlobal( "led", hwlib.GetLed( 0 ) );

    // Initialize debugging
    Debug debug;
    debug.Init( &script );
    debug.Add( "server", HttpServer::IsVerbose, HttpServer::SetVerbose );
    debug.Add( "client", HttpClient::IsVerbose, HttpClient::SetVerbose );
    debug.Add( "hwlib", HwLib::IsVerbose, HwLib::SetVerbose );
    debug.Add( "gpio", GPIO::IsVerbose, GPIO::SetVerbose );
    debug.Add( "clockmodule", ClockModule::IsVerbose, ClockModule::SetVerbose );
    debug.Add( "led", Led::IsVerbose, Led::SetVerbose );

    // Initialize the console
    Console console;
    console.Init( &script );
    a.connect( &console, SIGNAL( Done() ), &a, SLOT( quit() ) );

    return a.exec();
}

