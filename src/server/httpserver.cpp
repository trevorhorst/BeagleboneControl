#include "server/httpserver.h"

bool HttpServer::mVerbose = false;

std::string HttpServer::mServerKey = "";
std::string HttpServer::mServerCrt = "";

#define DEFAULT_SSL_CERTS_DIR   ":/ssl/certs/"
#define DEFAULT_ASSETS_DIR      ":/default_alt/assets/"

const char HttpServer::DEFAULT_BUNDLE_FILE[] = ":/default_alt/assets/bundle.js";
const char HttpServer::DEFAULT_INDEX_FILE[]  = ":/default_alt/assets/index.html";
const char HttpServer::DEFAULT_STYLE_FILE[]  = ":/default_alt/assets/stylesheet.css";

const QString HttpServer::DEFAULT_SERVER_CRT  = DEFAULT_SSL_CERTS_DIR  "sslserver-crt.pem";
const QString HttpServer::DEFAULT_SERVER_KEY  = DEFAULT_SSL_CERTS_DIR  "sslserver-key.pem";

static int AnswerToConnection (
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


    HttpServer *server = static_cast< HttpServer* >( cls );
    Request *request = static_cast< Request* >( *con_cls );

    if( request == NULL ) {
        // printf("On Request\n");
        return server->OnRequest( connection, method, url, con_cls );
    } else if( *upload_data_size > 0 ) {
        printf("On Request Body\n");
        return server->OnRequestBody( request, upload_data, upload_data_size );
    } else {
        printf("On Request Done\n");
        return server->OnRequestDone( request );
    }

}

static void OnResponseSent(
        void *cls
        , MHD_Connection *connection
        , void **request
        , MHD_RequestTerminationCode *rtc)
{
    // Unused
    (void)cls;
    (void)connection;
    (void)rtc;

    Request *r = static_cast< Request* >( *request );

    if( r->mPostProcessor != NULL ) {
        MHD_destroy_post_processor( r->mPostProcessor );
    }

    if( r->mFp ) {
        fclose( r->mFp );
    }

    r->deleteLater();
}


/** ****************************************************************************
 * @brief Constructor
 * @param port      Desired port to listen on
 * @param parent    Parent QObject
 * ****************************************************************************/
HttpServer::HttpServer( uint32_t port, QObject *parent )
    : QObject( parent )
    , mSecure( false )
    , mPort( port )
    , mStyleBuffer( NULL )
    , mBundleBuffer( NULL )
    , mIndexBuffer( NULL )
    , mThread()
{
    SetServerKey( DEFAULT_SERVER_KEY );
    SetServerCrt( DEFAULT_SERVER_CRT );

    mStyleBuffer  = ReadFile( DEFAULT_STYLE_FILE );
    mBundleBuffer = ReadFile( DEFAULT_BUNDLE_FILE );
    mIndexBuffer  = ReadFile( DEFAULT_INDEX_FILE );

    connect( this, SIGNAL( RequestReady( Request* ) )
             , this, SLOT( ProcessRequest( Request* ) )
             , Qt::DirectConnection );

    Start();
}

/** ****************************************************************************
 * @brief Destructor
 * ****************************************************************************/
HttpServer::~HttpServer()
{
    if( mStyleBuffer != NULL ) {
        delete[] mStyleBuffer;
    }

    if( mBundleBuffer != NULL ) {
        delete[] mBundleBuffer;
    }

    if( mIndexBuffer != NULL ) {
        delete[] mIndexBuffer;
    }

    if( mThread.isRunning() ) {
        mThread.quit();
        mThread.wait( 1000 );
    }
}

/** ****************************************************************************
 * @brief Starts the micro http daemon
 * @return  Bool indicating the success of the operation
 * ****************************************************************************/
bool HttpServer::Start()
{
    bool success = false;

    /// TODO: Need to figure out how to properly bind to an interface
    // sockaddr_in socket;
    // in_addr_t addr;
    // socket.sin_family = AF_INET;
    // socket.sin_port = htons( PORT );

    // QHostAddress address("192.168.1.8");
    // socket.sin_addr.s_addr = htonl( address.toIPv4Address() );


    // char *interface = "eth0";
    // int rc = setsockopt( socket, SOL_SOCKET, SO_BINDTODEVICE, interface
    //                      , (socklen_t)sizeof( interface ) );

    if( mSecure ) {
        mServerDaemon = MHD_start_daemon(
                    MHD_USE_POLL_INTERNALLY | MHD_USE_PEDANTIC_CHECKS | MHD_USE_THREAD_PER_CONNECTION | MHD_USE_SSL
                    , mPort
                    , NULL
                    , NULL
                    , &AnswerToConnection, this
                    , MHD_OPTION_NOTIFY_COMPLETED, &OnResponseSent, this
                    , MHD_OPTION_HTTPS_MEM_KEY, mServerKey.c_str()
                    , MHD_OPTION_HTTPS_MEM_CERT, mServerCrt.c_str()
                    , MHD_OPTION_END
                    );
    } else {
        mServerDaemon = MHD_start_daemon(
                    MHD_USE_POLL_INTERNALLY | MHD_USE_PEDANTIC_CHECKS | MHD_USE_THREAD_PER_CONNECTION
                    , mPort
                    , NULL
                    , NULL
                    , &AnswerToConnection, this
                    , MHD_OPTION_NOTIFY_COMPLETED, &OnResponseSent, this
                    // , MHD_OPTION_SOCK_ADDR, &socket
                    , MHD_OPTION_END
                    );
    }

    if( mServerDaemon == NULL ) {
        qWarning( "Server daemon failed to start" );
    } else {
        qDebug( "Server daemon started successfully" );
        if( mVerbose ){ qDebug( "Listening on port %d", PORT ); }
        success = true;
    }

    moveToThread( &mThread );
    mThread.start();

    return success;
}

/** ****************************************************************************
 * @brief Stops the micro http daemon
 * ****************************************************************************/
void HttpServer::Stop()
{
    if( mServerDaemon != NULL ) {
        qDebug( "Stopping Server..." );
        MHD_stop_daemon( mServerDaemon );
        mServerDaemon = NULL;
    }
}

/** ****************************************************************************
 * @brief Checks if the server is currently running
 * @return Boolean indicating the run status of the server
 * ****************************************************************************/
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

void HttpServer::SetSecure( bool secure )
{
    mSecure = secure;
}

bool HttpServer::SetInterface( QString interface )
{
    mInterface = interface;
    return true;
}

/** ****************************************************************************
 * @brief   Set the ssl key to be used by the server
 * @param   file  Desired ssl key
 * @return  Boolean indicating success of the operation
 * ****************************************************************************/
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

/** ****************************************************************************
 * @brief   Set the ssl crt to be used by the server
 * @param   file  Desired ssl crt
 * @return  Boolean indicating success of the operation
 * ****************************************************************************/
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

/** ****************************************************************************
 * @brief   Load a file to a buffer
 * @param   filename    Desired file
 * @param   buffer      Buffer to read into
 * @return  Boolean indicating the success of the operation
 * ****************************************************************************/
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

/** ****************************************************************************
 * @brief   Read dat stored in a file
 * @param   filename    File to read
 * @return  char pointer to the data read
 * ****************************************************************************/
char* HttpServer::ReadFile( const char *filename )
{
    QFile file( filename );
    if( file.exists() ) {
        uint32_t fileSize = file.size();
        file.open( QIODevice::ReadOnly );
        char* temp = new char[ fileSize + 1 ];
        file.read( temp, fileSize );
        temp[ fileSize ] = '\0';
        return temp;
    }

    return NULL;
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
    // Unused
    (void)( key );
    (void)( content_type );
    (void)( transfer_encoding );
    (void)( kind );
    (void)( off );

    Request *r = static_cast< Request* >( coninfo_cls );
    printf( "%s\n", qPrintable( r->mMethod ) );

    // printf( "%s\n", content_type );
    // printf( "%s\n", transfer_encoding );

    // con_info->answerstring = servererrorpage;
    // con_info->answercode = MHD_HTTP_INTERNAL_SERVER_ERROR;

    // if( 0 != strcmp( key, "file" ) ) {
    //     return MHD_NO;
    // }

    if( !r->mFp ) {
        // if( NULL != ( fp = fopen( filename, "rb" ) ) ) {
        //     fclose( fp );
        //     con_info->answerstring = fileexistspage;
        //     con_info->answercode = MHD_HTTP_FORBIDDEN;
        //     return MHD_NO;
        // }

        // r->mFp = fopen( filename, "ab" );
        r->mFp = fopen( filename, "wb" );
        if(!r->mFp ) {
            return MHD_NO;
        }
    }

    if( size > 0 ) {
        if( !fwrite( data, size, sizeof(char), r->mFp ) ) {
            return MHD_NO;
        }
    }

    // con_info->answerstring = completepage;
    // con_info->answercode = MHD_HTTP_OK;

    return MHD_YES;

}

/** ****************************************************************************
 * @brief Handles cleanup of a request once it has been answered
 * @param cls Extra paramters
 * @param connection Pointer to the connection object
 * @param con_cls Request object containg connection info
 * @param toe
 * ****************************************************************************/
void HttpServer::RequestCompleted(
        void *cls
        , MHD_Connection *connection
        , void **con_cls
        , MHD_RequestTerminationCode toe )
{
    (void)( cls );
    (void)( connection );
    (void)( toe );

    Request *con_info = (Request*)*con_cls;

    if( NULL == con_info ) {
        return;
    }

    if( strcmp( con_info->mData, "POST" ) == 0 ) {
        if( NULL != con_info->mPostProcessor ) {
              MHD_destroy_post_processor( con_info->mPostProcessor );
        }

        if( con_info->mFp ) {
          fclose( con_info->mFp );
        }
    }

    free( con_info );
    *con_cls = NULL;
}

/** ****************************************************************************
 * @brief Stores header values of the request in a hash table
 * @param cls Extra paramaters
 * @param kind
 * @param key Key value of the header item
 * @param value Details of the header item
 * @return Status value of the operation
 * ****************************************************************************/
int HttpServer::HeaderValuesIterator(
        void *cls
        , MHD_ValueKind kind
        , const char *key
        , const char *value )
{
    // Unused
    (void)kind;

    QHash< const char*, const char* > *headers
            = static_cast< QHash< const char*, const char* >* >( cls );
    headers->insert( key, value );
    return MHD_YES;
}

/** ****************************************************************************
 * @brief Handles a new incoming request
 * @param connection Pointer to the connection object
 * @param method RESTful API HTTP method
 * @param path URL of the incoming connection
 * @param request
 * @return Status value of the operation
 * ****************************************************************************/
int HttpServer::OnRequest(MHD_Connection *connection
        , const char *method
        , const char *path
        , void **request )
{
    Request *r = new Request( connection );
    MHD_get_connection_values( connection, MHD_HEADER_KIND
                               , &HeaderValuesIterator, &r->mHeaders );
    r->mPostProcessor = MHD_create_post_processor(
                connection
                , 512
                , IteratePost
                , (void*)r );
    if( r->mPostProcessor == NULL ) {
        printf( "PostProcessor is NULL\n" );
    }

    r->mFp       = NULL;
    r->mMethod   = method;
    r->mUrl      = path;
    r->mData     = NULL;
    r->mDataSize = 0;

    *request = r;

    return MHD_YES;
}

/** ****************************************************************************
 * @brief Handles incoming request data
 * @param request Requst containing connection info
 * @param data Incoming data
 * @param size Size of the incoming data
 * @return Status value of the operation
 * ****************************************************************************/
int HttpServer::OnRequestBody(
        Request *request
        , const char *data
        , size_t *size )
{
    request->mBody.append( data );

    request->mDataSize += *size;
    if( request->mData == NULL ) {
        // If the data is null lets copy over the incoming data
        request->mData = new char[ *size ];
        memcpy( (void*)request->mData, data, *size );
    } else {
        // Concatinate new data with the old data
        const char* t = request->mData;
        request->mData = new char[ request->mDataSize ];
        memcpy( (void*)request->mData, t, request->mDataSize - *size );
        const char* n = &request->mData[ request->mDataSize - *size ];
        memcpy( (void*)n, data, *size );
        delete[] t;
    }

    *size = 0;
    return MHD_YES;
}

/** ****************************************************************************
 * @brief Signal that we have finished building a request
 * @param request Request containng connection info
 * @return Status value of the operation
 * ****************************************************************************/
int HttpServer::OnRequestDone( Request *request )
{
    emit RequestReady( request );
    return MHD_YES;
}

/** ****************************************************************************
 * @brief Handles received request data
 * @param request Request containing connection info
 * ****************************************************************************/
void HttpServer::ProcessRequest( Request *request )
{
    QList< const char * > keys = request->mHeaders.keys();

    if( mVerbose ) {
        printf("-----------------------------------------------------------\n");
        for( int i = 0; i < keys.length(); i++ ) {
            printf( "%20s: %s\n", keys[ i ], request->mHeaders.value( keys[ i ] ) );
        }
        printf("-----------------------------------------------------------\n");
    }

    // Handle GET requests
    if( request->mMethod == "GET" ) {
        if( request->mUrl == "/" || request->mUrl == "/index.html" ) {
            // Send the index page
            request->SendResponse( mIndexBuffer, "text/html", MHD_HTTP_OK );
        } else if( request->mUrl == "/bundle.js" ) {
            // Send the script file
            request->SendResponse( mBundleBuffer, "text/javascript", MHD_HTTP_OK );
        } else if( request->mUrl == "/stylesheet.css" ) {
            // Send the style file
            request->SendResponse( mStyleBuffer, "text/css", MHD_HTTP_OK );
        }
    }

    // Handle POST requests
    if( request->mMethod == "POST" ) {

        if( request->mPostProcessor != NULL ) {
            // If the post processor isn't NULL, just let libmicro handle the
            // data
            printf( "Handling post process data\n" );

            MHD_post_process( request->mPostProcessor
                              , request->mData
                              , request->mDataSize );
            // if( request->mFp != NULL ) {
            //     fclose( request->mFp );
            //     request->mFp = NULL;
            // }
        } else {
            // If there isn't a post processor we will have to handle the data
            // ourselves

            // Get the body data
            QString body = QString::fromUtf8( request->mData, request->mDataSize );
            if( mVerbose ){ printf("%s", qPrintable( body ) ); }

            // Parse the command as JSON
            QVariantMap command = ParseJson( body );

            // Check the command type
            QString type = command.value( "cmd" ).toString();
            if( mCommandHandler->mCommandList.contains( type ) ) {
                // If the command type is available
                Command *cmd = mCommandHandler->mCommandList.value( type );
                if( command.contains( "parameters" ) ) {
                    // If there are parameters, then we are setting values
                    QVariantMap parameters = command.value( "parameters" ).toMap();
                    cmd->Set( parameters );
                } else {
                    // If there are parameters, then we are querying
                    cmd->Query();
                }

            }

            // Send response to post
            QJsonObject message;
            message.insert( "rsp", QJsonValue::fromVariant( "success" ) );
            QJsonDocument response( message );
            request->SendResponse( response.toJson().toStdString().c_str()
                                   , "application/json"
                                   , MHD_HTTP_OK );

        }

    }

    if( mVerbose ) {
        printf("-----------------------------------------------------------\n");
    }

    request->SendResponse( errorpage, "text/html", MHD_HTTP_BAD_REQUEST );
}

QVariantMap HttpServer::ParseJson( QString data )
{
    QJsonParseError error;
    QJsonDocument command = QJsonDocument::fromJson( data.toLocal8Bit(), &error );
    if( error.error == QJsonParseError::GarbageAtEnd ) {
        // Ignore garbage
        for( uint32_t i = 0; i < data.size(); i++ ) {
            printf( "%d: %X\n", i, data.at( i ) );
        }
        printf( "\n" );
    } else if( error.error != QJsonParseError::NoError ) {
        qWarning( "JSON Error: %s", qPrintable( error.errorString() ) );
        return QVariantMap();
    }

    return command.object().toVariantMap();
}

void HttpServer::SetCommandHandler( CommandHandler *cmdHndlr )
{
    mCommandHandler = cmdHndlr;
}
