#ifndef REQUEST_H
#define REQUEST_H

#include <QObject>
#include <QHash>

#include <microhttpd.h>

class Request : public QObject
{
    Q_OBJECT
public:
    explicit Request( MHD_Connection *connection );


    void SetMethod( QString method );
    void SetUrl( QString url );

    MHD_Connection *mConnection;
    MHD_PostProcessor *mPostProcessor;
    QHash< const char*, const char *> mHeaders;
    QString mMethod;
    QString mUrl;
    QString mBody;
    const char* mData;
    uint32_t mDataSize;
    FILE* mFp;

    const char* GetBody();
signals:
    void Finished();

public slots:
    int SendResponse( const char *responseData
                      , const char *responseType
                      , int statusCode );

    void HandleRequest();

private:
};

#endif // REQUEST_H
