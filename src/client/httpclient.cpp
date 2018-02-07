#include "httpclient.h"

#include <unistd.h>

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    mManager = new QNetworkAccessManager();
    mReply = NULL;

    mConfig = QSslConfiguration::defaultConfiguration();
    mConfig.setProtocol( QSsl::TlsV1_2 );

    QFile cert( ":/ssl/ca-crt.pem" );
    cert.open( QIODevice::ReadOnly );
    mCert = QSslCertificate( &cert, QSsl::Pem );

    QList<QSslCertificate> caCerts = mConfig.caCertificates();
    caCerts.append( mCert );
    mConfig.setCaCertificates( caCerts );

    connect( mManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>))
             , this, SLOT(HandleError(QNetworkReply*,QList<QSslError>)));

    connect( mManager, SIGNAL( finished( QNetworkReply* ) )
             , this, SLOT( ReplyFinished( QNetworkReply* ) ) );

    mRequest.setSslConfiguration( mConfig );
    mRequest.setUrl( QUrl( "https://localhost:19091/api/command" ) );
    mRequest.setHeader( QNetworkRequest::ServerHeader, "application/json" );

    connect( this, SIGNAL(SignalSendRequest())
             , this, SLOT(SendRequest()) );


    // QString json = "{\"cmd\":\"qstatus\",\"msg\":0}";
}

void HttpClient::SendRequest()
{
    QJsonObject json;
    json.insert( "cmd", "qstatus" );
    json.insert( "msg", 0 );
    // mManager->get( mRequest );
    // usleep( 1000 );
    mManager->post( mRequest
                    , QJsonDocument( json ).toJson(QJsonDocument::Compact) );
}

void HttpClient::HandleError(QNetworkReply *reply, QList<QSslError> errors )
{
    for( int i = 0; i < errors.count(); i++ ) {
        qDebug( qPrintable( errors[ 0 ].errorString() ) );
    }
}

void HttpClient::ReplyFinished(QNetworkReply *reply)
{
    qDebug( "ReplyFinished" );
    qDebug( reply->readAll() );

    //qDebug() << reply->readAll();

}

