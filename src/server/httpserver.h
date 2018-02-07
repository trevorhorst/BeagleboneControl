/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <QObject>

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>

#include <QFile>
#include <QHostAddress>

#define PORT            8888
#define POSTBUFFERSIZE  512
#define MAXCLIENTS      2

#define GET             0
#define POST            1

static unsigned int nr_of_uploading_clients = 0;

struct ConnectionInfo
{
  int connectiontype;
  struct MHD_PostProcessor *postprocessor;
  FILE *fp;
  const char *answerstring;
  int answercode;
};


static const char *busypage =
  "<html><body>This server is busy, please try again later.</body></html>";

static const char *completepage =
  "<html><body>The upload has been completed.</body></html>";

static const char *errorpage =
  "<html><body>This doesn't seem to be right.</body></html>";
static const char *servererrorpage =
  "<html><body>An internal server error has occured.</body></html>";
static const char *fileexistspage =
  "<html><body>This file already exists.</body></html>";

class HttpServer : public QObject
{
    Q_OBJECT
public:
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose = true ) { mVerbose = verbose; }

    explicit HttpServer( QObject *parent = 0 );
    ~HttpServer();
    static bool LoadFile( const QString &file, QString &buffer );
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
    static int AnswerToConnection (
            void *cls
            , struct MHD_Connection *connection
            , const char *url, const char *method
            , const char *version, const char *upload_data
            , size_t *upload_data_size
            , void **con_cls );

    static int PrintOutKey(
            void *cls
            , enum MHD_ValueKind kind
            , const char *key
            , const char *value)
    {
        (void)( cls );
        (void)( kind );
        printf( "%s: %s\n", key, value );
        return MHD_YES;
    }


    static bool SetBundleFile( QString file );
    static bool SetIndexFile( QString file );
    static bool SetStyleFile( QString file );

    static bool SetServerKey( QString file );
    static bool SetServerCrt( QString file );

    void SetSecure( bool secure = true ) { mSecure = secure; }
    bool SetInterface( QString interface );

signals:
public slots:
    bool Start();
    void Stop();
    bool IsRunning();

private:
    static bool mVerbose;
    static bool mSecure;

    static const QString DEFAULT_ASSETS_DIR;
    static const QString DEFAULT_INDEX_FILE;
    static const QString DEFAULT_BUNDLE_FILE;
    static const QString DEFAULT_STYLE_FILE;

    static const QString DEFAULT_SSL_CERTS_DIR;
    static const QString DEFAULT_SERVER_KEY;
    static const QString DEFAULT_SERVER_CRT;


    MHD_Daemon *mServerDaemon;
    unsigned int mPort;
    QHostAddress mHostAddress;
    QString mInterface;
    static std::string mIndexPage;
    static std::string mBundlePage;
    static std::string mStylePage;
    static std::string mTestPage;
    static std::string mServerKey;
    static std::string mServerCrt;
};


#endif //HTTPSERVER_H
