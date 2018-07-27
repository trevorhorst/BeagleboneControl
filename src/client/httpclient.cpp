#include "httpclient.h"

#include <unistd.h>

bool HttpClient::mVerbose = true;

/**
 * @brief HttpClient::HttpClient Constructor
 * @param parent Parent object
 */
HttpClient::HttpClient( QObject *parent )
    : QObject(parent)
    , mSecure( false )
{
    mManager = new QNetworkAccessManager();
    mReply = NULL;

    // Configure the TLS V1.2
    mConfig = QSslConfiguration::defaultConfiguration();
    mConfig.setProtocol( QSsl::TlsV1_2 );

    // Configure the SSL certificate authority
    QFile cert( ":/ssl/ca-crt.pem" );
    cert.open( QIODevice::ReadOnly );
    mCert = QSslCertificate( &cert, QSsl::Pem );

    // Add the SSL certificate authority to the configuration
    QList<QSslCertificate> caCerts = mConfig.caCertificates();
    caCerts.append( mCert );
    mConfig.setCaCertificates( caCerts );

    // Configure the SSL
    mRequest.setSslConfiguration( mConfig );
    // mRequest.setUrl( QUrl( "http://localhost:8000/" ) );
    SetUrl( "localhost/", 8000 );
    mRequest.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );

    // Connect the error handler
    connect( mManager, SIGNAL( sslErrors( QNetworkReply*, QList<QSslError> ) )
             , this, SLOT( HandleError( QNetworkReply*,QList<QSslError> ) ) );

    // Connect the finished response handler
    connect( mManager, SIGNAL( finished( QNetworkReply* ) )
             , this, SLOT( ReplyFinished( QNetworkReply* ) ) );

    // Connect the signal handler for request sending
    connect( this, SIGNAL( SignalSendRequest( QByteArray ) )
             , this, SLOT( SendRequest( QByteArray ) ) );

}

/**
 * @brief HttpClient::SetSecure Option to enable SSL security on the server
 * @param secure SSL enable/disable option
 */
void HttpClient::SetSecure( bool secure )
{
    mSecure = secure;
}

/**
 * @brief HttpClient::SetUrl
 * @param address Destination address to post the request
 * @param port Destination port to post the request
 * @return Boolean indicating success of the operation
 */
bool HttpClient::SetUrl( QString address, uint16_t port )
{
    QString url = "";
    if( mSecure ) {
        url.append( "https://" );
    } else {
        url.append( "http://" );
    }

    url.append( address );

    if( port > 0 ) {
        url.append( QString( ":%0" ).arg( port ) );
    } else {
        return false;
    }

    if( mVerbose ){ qDebug( "%s: %s", __FILE__ , qPrintable( url ) ); }
    mRequest.setUrl( QUrl( url ) );
    return true;
}

/**
 * @brief HttpClient::SendRequest Send data out over a post request
 * @param data Desired data to send
 */
void HttpClient::SendRequest( QByteArray data )
{
    if( mVerbose ) {
        qDebug( qPrintable( __FUNCTION__ ) );
    }
    // mManager->post( mRequest
    //                 , QJsonDocument( json ).toJson(QJsonDocument::Compact) );
    mManager->post( mRequest, data );
}

/**
 * @brief HttpClient::BuildCommand Build a command of provided parameters and a
 * command
 * @param type Command type
 * @param params Parameters to accompany the command
 */
void HttpClient::BuildCommand( QString type, QVariantList params )
{
    if( mVerbose ) {
        qDebug( qPrintable( __FUNCTION__ ) );
    }
    // QJsonObject jsonCmd;
    // QJsonObject jsonParams;
    //
    // jsonCmd.insert( "cmd", type );
    // qDebug( "Command %s | Parameters %d", qPrintable( type ), params.size() );
    QVariantMap paramMap;
    for( quint32 i = 0; i < params.size(); i+=2 ) {
        QString key = params.at( i ).toString();
        if( key.length() == 0 ) {
            break;
        }
        if( i + 1 >= params.length() ) {
            break;
        }

        QVariant val = params.at( i + 1 );


        paramMap.insert( key, val );
        // qDebug( "Param: %s
        //         , qPrintable( params.at( i ).toString() ) );
    }

    QVariantMap map;
    map.insert( "msg", 0 );
    map.insert( "cmd", type );
    map.insert( "parameters", paramMap );

    QJsonDocument json = QJsonDocument::fromVariant( map );
    QByteArray arr = json.toJson( QJsonDocument::Compact );

    emit SignalSendRequest( arr );
    // mManager->post( mRequest, arr );
}

void HttpClient::HandleError( QNetworkReply *reply, QList<QSslError> errors )
{
    (void)( reply );

    for( int i = 0; i < errors.count(); i++ ) {
        qDebug( qPrintable( errors[ i ].errorString() ) );
    }
}

void HttpClient::ReplyFinished( QNetworkReply *reply )
{
    printf("-----------------------------------------------------------\n");
    qDebug( "%s", qPrintable( reply->readAll() ) );
    printf("-----------------------------------------------------------\n");

    //qDebug() << reply->readAll();

}

