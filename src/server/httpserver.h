/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QVariantMap>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>

#include <QThread>
#include <QList>
#include <QFile>
#include <QHostAddress>

#include "server/request.h"
#include "server/commandhandler.h"
#include "server/cmdled.h"

#define PORT            8888
#define POSTBUFFERSIZE  512
#define MAXCLIENTS      2

#define GET             0
#define POST            1

struct ConnectionInfo
{
  int connectiontype;
  struct MHD_PostProcessor *postprocessor;
  FILE *fp;
  const char *answerstring;
  int answercode;
};


static const char *errorpage =
  "<html><body>This doesn't seem to be right.</body></html>";

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo!!</body></html>"


class HttpServer : public QObject
{
    Q_OBJECT
public:
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose = true ) { mVerbose = verbose; }

    explicit HttpServer( uint32_t port = 8000, QObject *parent = 0 );
    ~HttpServer();
    static bool LoadFile( const QString &file, QString &buffer );

    bool LoadFileToBuffer(const char *filename, char* buffer );
    char* ReadFile( const char *filename );

    static int SendPage(MHD_Connection *connection
            , std::string page
            , int status_code , std::string page_type = "text/html");
    static int IteratePost(
            void *coninfo_cls
            , MHD_ValueKind kind
            , const char *key
            , const char *filename
            , const char *content_type
            , const char *transfer_encoding
            , const char *data
            , uint64_t off
            , size_t size );
    static void RequestCompleted(
            void *cls
            , MHD_Connection *connection
            , void **con_cls
            , MHD_RequestTerminationCode toe );
    // static int AnswerToConnection (
    //         void *cls
    //         , struct MHD_Connection *connection
    //         , const char *url, const char *method
    //         , const char *version, const char *upload_data
    //         , size_t *upload_data_size
    //         , void **con_cls );

    static int HeaderValuesIterator(
            void *cls
            , enum MHD_ValueKind kind
            , const char *key
            , const char *value);

    // static void OnResponseSent( void *cls, MHD_Connection *connection
    //                             , void **request, MHD_RequestTerminationCode *rtc );

    static bool SetServerKey( QString file );
    static bool SetServerCrt( QString file );

    bool SetInterface( QString interface );

    int OnRequest(MHD_Connection *connection
            , const char *method
            , const char *path
            , void **request );

    int OnRequestBody(Request *request
            , const char *data
            , size_t *size );

    int OnRequestDone(
            Request *request );

    QVariantMap ParseJson( QString data );
    void SetCommandHandler( CommandHandler *cmdHndlr );

signals:
    void RequestReady( Request *request );
public slots:
    bool Start();
    void Stop();
    bool IsRunning();
    void ProcessRequest( Request *request );
    void SetSecure( bool secure = true );

private:
    static bool mVerbose;
    bool mSecure;

    static const QString DEFAULT_ASSETS_DIR;
    static const char DEFAULT_INDEX_FILE[];
    static const char DEFAULT_BUNDLE_FILE[];
    static const char DEFAULT_STYLE_FILE[];

    static const QString DEFAULT_SSL_CERTS_DIR;
    static const QString DEFAULT_SERVER_KEY;
    static const QString DEFAULT_SERVER_CRT;


    MHD_Daemon *mServerDaemon;
    unsigned int mPort;
    QHostAddress mHostAddress;
    QString mInterface;
    static std::string mTestPage;
    static std::string mServerKey;
    static std::string mServerCrt;

    char* mStyleBuffer;
    char* mBundleBuffer;
    char* mIndexBuffer;

    CommandHandler *mCommandHandler;

    QThread mThread;
};


#endif //HTTPSERVER_H
