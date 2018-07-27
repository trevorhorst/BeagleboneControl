#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QVariantMap>


class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(QObject *parent = nullptr);
    virtual ~Command(){ }
    virtual bool Set( QVariantMap parameterList ) = 0;
    virtual bool Query() = 0;

    static bool mVerbose;
    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose ){ mVerbose = verbose; }

signals:

public slots:
};

#endif // COMMAND_H
