#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <stdint.h>

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonObject>
#include <QJsonDocument>

#include <QFile>
#include <QUrl>


class HttpClient : public QObject
{
    Q_OBJECT
public:
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose = true ) { mVerbose = verbose; }

    explicit HttpClient( QObject *parent = 0 );

signals:
    void SignalSendRequest( QByteArray data );

public slots:
    void SendRequest( QByteArray data );
    void BuildCommand( QString type, QVariantList params );
    void HandleError( QNetworkReply *reply, QList<QSslError> errors );
    void ReplyFinished( QNetworkReply *reply );

    void SetSecure( bool secure = true );
    bool SetUrl(QString address , uint16_t port = 0 );
private:
    static bool mVerbose;
    bool mSecure;

    QNetworkAccessManager *mManager;
    QNetworkRequest mRequest;
    QNetworkReply *mReply;

    QSslConfiguration mConfig;
    QSslCertificate mCert;
};

#endif // HTTPCLIENT_H
