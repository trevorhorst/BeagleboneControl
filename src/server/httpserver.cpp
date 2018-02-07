#include "server/httpserver.h"

bool HttpServer::mVerbose = true;
bool HttpServer::mSecure  = false;

std::string HttpServer::mIndexPage = "";
std::string HttpServer::mBundlePage = "";
std::string HttpServer::mStylePage = "";
std::string HttpServer::mServerKey = "";
std::string HttpServer::mServerCrt = "";

const QString HttpServer::DEFAULT_ASSETS_DIR  = ":/default_alt/assets/";
const QString HttpServer::DEFAULT_BUNDLE_FILE = DEFAULT_ASSETS_DIR + "bundle.js";
const QString HttpServer::DEFAULT_INDEX_FILE  = DEFAULT_ASSETS_DIR + "index.html";
const QString HttpServer::DEFAULT_STYLE_FILE  = DEFAULT_ASSETS_DIR + "stylesheet.css";

const QString HttpServer::DEFAULT_SSL_CERTS_DIR = ":/ssl/certs/";
const QString HttpServer::DEFAULT_SERVER_CRT  = DEFAULT_SSL_CERTS_DIR + "sslserver-crt.pem";
const QString HttpServer::DEFAULT_SERVER_KEY  = DEFAULT_SSL_CERTS_DIR + "sslserver-key.pem";

HttpServer::HttpServer( QObject *parent )
    : QObject( parent )
{
    SetIndexFile( DEFAULT_INDEX_FILE );
    SetBundleFile( DEFAULT_BUNDLE_FILE );
    SetStyleFile( DEFAULT_STYLE_FILE );

    SetServerKey( DEFAULT_SERVER_KEY );
    SetServerCrt( DEFAULT_SERVER_CRT );

}

HttpServer::~HttpServer()
{
}

bool HttpServer::Start()
{
    bool success = false;
    sockaddr_in socket;
    in_addr_t addr;
    socket.sin_family = AF_INET;
    socket.sin_port = htons( PORT );

    QHostAddress address("192.168.1.8");
    socket.sin_addr.s_addr = htonl( address.toIPv4Address() );


    // char *interface = "eth0";
    // int rc = setsockopt( socket, SOL_SOCKET, SO_BINDTODEVICE, interface
    //                      , (socklen_t)sizeof( interface ) );

    if( mSecure ) {
        mServerDaemon = MHD_start_daemon(
                    MHD_USE_SELECT_INTERNALLY | MHD_USE_SSL
                    , PORT
                    , NULL
                    , NULL
                    , &HttpServer::AnswerToConnection
                    , NULL
                    , MHD_OPTION_NOTIFY_COMPLETED
                    , HttpServer::RequestCompleted
                    , NULL
                    , MHD_OPTION_HTTPS_MEM_KEY
                    , mServerKey.c_str()
                    , MHD_OPTION_HTTPS_MEM_CERT
                    , mServerCrt.c_str()
                    , MHD_OPTION_END
                    );
    } else {
        mServerDaemon = MHD_start_daemon(
                    MHD_USE_SELECT_INTERNALLY
                    , PORT
                    , NULL
                    , NULL
                    , &HttpServer::AnswerToConnection
                    , NULL
                    , MHD_OPTION_NOTIFY_COMPLETED
                    , HttpServer::RequestCompleted
                    , NULL
                    , MHD_OPTION_SOCK_ADDR
                    , &socket
                    , MHD_OPTION_END
                    );
    }

    if( mServerDaemon == NULL ) {
        qWarning( "Server daemon failed to start" );
    } else {
        qDebug( "Server daemon started successfully" );
        if( mVerbose ){ qDebug( "Listening on %s:%d", qPrintable( address.toString() ),  PORT ); }

        success = true;
    }

    return success;
}

void HttpServer::Stop()
{
    if( mServerDaemon != NULL ) {
        qDebug( "Stopping Server..." );
        MHD_stop_daemon( mServerDaemon );
        mServerDaemon = NULL;
    }
}

/**
 * @brief Checks if the server is currently running
 * @return Boolean indicating the run status of the server
 */
bool HttpServer::IsRunning()
{
    // If the server is NULL then we definitely aren't running.
    if( mServerDaemon == NULL ) {
        return false;
    }

    const MHD_DaemonInfo *info = MHD_get_daemon_info(
                mServerDaemon, MHD_DAEMON_INFO_LISTEN_FD );
    if( mVerbose ){ qDebug( "Daemon Socket FD: %d", info->listen_fd ); }
    if( mVerbose ){ qDebug( "Number of connections: %d", info->num_connections ); }
    if( info->listen_fd >= 0 ) {
        return true;
    }
    return false;
}

bool HttpServer::SetInterface( QString interface )
{
    mInterface = interface;
    return true;
}

/**
 * @brief Set the bundle script file to be used by the server
 * @param file Desired bundle file
 * @return Boolean indicating success of the operation
 */
bool HttpServer::SetBundleFile( QString file )
{
    bool success = false;
    QString buffer;
    if( LoadFile( file, buffer ) ) {
        mBundlePage = buffer.toStdString();
        success = true;
    } else {
        qWarning( "Failed to set the bundle page" );
    }
    return success;
}

/**
 * @brief Set the index script file to be used by the server
 * @param file Desired index file
 * @return Boolean indicating success of the operation
 */
bool HttpServer::SetIndexFile( QString file )
{
    bool success = false;
    QString buffer;
    if( LoadFile( file, buffer ) ) {
        mIndexPage = buffer.toStdString();
        success = true;
    } else {
        qWarning( "Failed to set the index page" );
    }
    return success;
}

/**
 * @brief Set the style file to be used by the server
 * @param file Desired style file
 * @return Boolean indicating success of the operation
 */
bool HttpServer::SetStyleFile( QString file )
{
    bool success = false;
    QString buffer;
    if( LoadFile( file, buffer ) ) {
        mStylePage = buffer.toStdString();
        success = true;
    } else {
        qWarning( "Failed to set the style page" );
    }
    return success;
}

/**
 * @brief Set the ssl key to be used by the server
 * @param file Desired ssl key
 * @return Boolean indicating success of the operation
 */
bool HttpServer::SetServerKey( QString file )
{
    bool success = false;
    QString buffer;
    if( LoadFile( file, buffer ) ) {
        mServerKey = buffer.toStdString();
        success = true;
    } else {
        qWarning( "Failed to set the server key" );
    }
    return success;
}

/**
 * @brief Set the ssl crt to be used by the server
 * @param file Desired ssl crt
 * @return Boolean indicating success of the operation
 */
bool HttpServer::SetServerCrt( QString file )
{
    bool success = false;
    QString buffer;
    if( LoadFile( file, buffer ) ) {
        mServerCrt = buffer.toStdString();
        success = true;
    } else {
        qWarning( "Failed to set the server crt" );
    }
    return success;
}

/**
 * @brief Load a file to a buffer
 * @param filename Desired file
 * @param buffer Buffer to read into
 * @return Boolean indicating the success of the operation
 */
bool HttpServer::LoadFile( const QString &filename, QString &buffer )
{
    bool success = false;

    QFile file( filename );
    if( file.exists() ) {
        file.open( QIODevice::ReadOnly );
        buffer = file.readAll();
        file.close();
        success = true;
    } else {
        qWarning( "Failed to load file: %s", qPrintable( filename ) );
        success = false;
    }
    return success;
}

int HttpServer::SendPage(
        MHD_Connection *connection
        , std::string page
        , int status_code
        , std::string page_type)
{
    int ret;
    struct MHD_Response *response;

    const char* cpage = page.c_str();
    response = MHD_create_response_from_buffer(
                strlen( cpage )
                , (void*)( cpage )
                , MHD_RESPMEM_MUST_COPY );

    if( !response ) {
        return MHD_NO;
    }
    MHD_add_response_header( response
                             , MHD_HTTP_HEADER_CONTENT_TYPE
                             , page_type.c_str() );
    ret = MHD_queue_response( connection, status_code, response );
    MHD_destroy_response( response );

    return ret;
}

int HttpServer::IteratePost(
        void *coninfo_cls
        , MHD_ValueKind kind
        , const char *key
        , const char *filename
        , const char *content_type
        , const char *transfer_encoding
        , const char *data
        , uint64_t off, size_t size )
{
    (void)( kind );
    (void)( off );

    struct ConnectionInfo *con_info
            = (ConnectionInfo*)coninfo_cls;
    FILE *fp;
    printf( "%s\n", content_type );
    printf( "%s\n", transfer_encoding );

    con_info->answerstring = servererrorpage;
    con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;

    if( 0 != strcmp( key, "file" ) ) {
        return MHD_NO;
    }

    if( !con_info->fp ) {
        if( NULL != ( fp = fopen( filename, "rb" ) ) ) {
            fclose( fp );
            con_info->answerstring = fileexistspage;
            con_info->answercode = MHD_HTTP_FORBIDDEN;
            return MHD_NO;
        }

        con_info->fp = fopen( filename, "ab" );
        if(!con_info->fp ) {
            return MHD_NO;
        }
    }

    if( size > 0 ) {
        if( !fwrite( data, size, sizeof(char), con_info->fp ) ) {
            return MHD_NO;
        }
    }

    con_info->answerstring = completepage;
    con_info->answercode = MHD_HTTP_OK;

    return MHD_YES;

}

void HttpServer::RequestCompleted(
        void *cls
        , MHD_Connection *connection
        , void **con_cls
        , MHD_RequestTerminationCode toe )
{
    (void)( cls );
    (void)( connection );
    (void)( toe );

    ConnectionInfo *con_info = (ConnectionInfo*)*con_cls;

    if( NULL == con_info ) {
        return;
    }

    if( con_info->connectiontype == POST) {
        if( NULL != con_info->postprocessor ) {
              MHD_destroy_post_processor( con_info->postprocessor );
              nr_of_uploading_clients--;
        }

        if( con_info->fp ) {
          fclose( con_info->fp );
        }
    }

    free( con_info );
    *con_cls = NULL;
}

int HttpServer::AnswerToConnection (
        void *cls
        , MHD_Connection *connection
        , const char *url, const char *method
        , const char *version, const char *upload_data
        , size_t *upload_data_size
        , void **con_cls
        )
{
    // Unused
    ( void )( cls );
    ( void )( version );
    // printf ("New %s request for %s using version %s\n", method, url, version);

    // MHD_get_connection_values( connection, MHD_HEADER_KIND, print_out_key,
    //                            NULL);

    if( NULL == *con_cls ) {
        struct ConnectionInfo *con_info;

        if( nr_of_uploading_clients >= MAXCLIENTS ) {
            return SendPage( connection, busypage
                              , MHD_HTTP_SERVICE_UNAVAILABLE );
        }

        con_info = (ConnectionInfo*)malloc(
                    sizeof ( struct ConnectionInfo ) );
        if( NULL == con_info ) {
            return MHD_NO;
        }

        con_info->fp = NULL;

        if( 0 == strcmp( method, "POST" ) ) {
            con_info->postprocessor =
              MHD_create_post_processor (connection, POSTBUFFERSIZE,
                                         IteratePost, (void *) con_info);

            if (NULL == con_info->postprocessor) {
                free( con_info );
                return MHD_NO;
            }

            nr_of_uploading_clients++;

            con_info->connectiontype = POST;
            con_info->answercode = MHD_HTTP_OK;
            con_info->answerstring = completepage;
        } else {
            con_info->connectiontype = GET;
        }

        *con_cls = (void *) con_info;

        return MHD_YES;
    }

    if (0 == strcmp (method, "GET"))
    {
        if( 0 == strcmp( url, "/" ) || 0 == strcmp( url, "/index.html" ) ) {
            // Send the index page
            return SendPage( connection, mIndexPage, MHD_HTTP_OK );
        } else if( 0 == strcmp( url, "/bundle.js" ) ) {
            // Send the script file
            return SendPage( connection, mBundlePage, MHD_HTTP_OK, "text/javascript" );
        } else if( 0 == strcmp( url, "/stylesheet.css" ) ) {
            // Send the style file
            return SendPage( connection, mStylePage, MHD_HTTP_OK, "text/css" );
        }
    }

    if (0 == strcmp (method, "POST"))
    {
        struct ConnectionInfo *con_info = (ConnectionInfo*)*con_cls;

        if( 0 != *upload_data_size ) {
            MHD_post_process (con_info->postprocessor, upload_data,
                              *upload_data_size);
            *upload_data_size = 0;

            return MHD_YES;
        } else  {
          if (NULL != con_info->fp)
          {
            fclose (con_info->fp);
            con_info->fp = NULL;
          }
          /* Now it is safe to open and inspect the file before calling SendPage with a response */
          return SendPage (connection, con_info->answerstring,
                    con_info->answercode);
        }
    }

    return SendPage (connection, errorpage, MHD_HTTP_BAD_REQUEST);
}
