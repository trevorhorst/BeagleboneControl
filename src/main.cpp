#include <QCoreApplication>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QHostAddress>
#include <QRegExp>
#include <QStringList>
#include <QFile>
#include <QList>

#include <qhttpengine/filesystemhandler.h>
#include <qhttpengine/handler.h>
#include <qhttpengine/server.h>
#include <qhttpengine/qobjecthandler.h>

#include "server/httpserver.h"

// #include <QHttpEngine/QFilesystemHandler>
// #include <QHttpEngine/QHttpHandler>
// #include <QHttpEngine/QHttpServer>
// #include <QHttpEngine/QObjectHandler>

// #include "apihandler.h"
// #include "HttpServer.h"
// #include "Transceiver.h"
// #include "ndr354server.h"
// #include "httpclient.h"

#include "common/console.h"
#include "common/debug.h"
#include "common/script.h"

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

    // Obtain the values
    // QHostAddress address = QHostAddress(parser.value(addressOption));
    // quint16 port = parser.value(portOption).toInt();
    // QString interface = parser.value( interfaceOption ).toHtmlEscaped();
    // QString address = "127.0.0.1";

    // Initialize the server
    // NDR354Server *server = new NDR354Server( interface, address, port );
    HttpServer *server = new HttpServer();

    // Initialize scripting
    Script script;
    script.AddGlobal( "server", server );

    // Initialize debugging
    Debug debug;

    debug.Init( &script );

    // Initialize the console
    Console console;
    console.Init( &script );
    a.connect( &console, SIGNAL( Done() ), &a, SLOT( quit() ) );

    return a.exec();
}

