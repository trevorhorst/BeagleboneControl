#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

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
    explicit HttpClient(QObject *parent = 0);

signals:
    void SignalSendRequest();

public slots:
    void SendRequest();
    void HandleError( QNetworkReply *reply, QList<QSslError> errors );
    void ReplyFinished( QNetworkReply *reply );
private:
    QNetworkAccessManager *mManager;
    QNetworkRequest mRequest;
    QNetworkReply *mReply;

    QSslConfiguration mConfig;
    QSslCertificate mCert;
};

#endif // HTTPCLIENT_H
