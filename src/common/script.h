#ifndef SCRIPT_H
#define SCRIPT_H

#include <QObject>
#include <QMap>

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

#include <stdio.h>
#define DEBUG(X) if( Script::IsVerbose() ){ printf( "Script: %s\n", qPrintable( X ) ); }

class Script : public QObject
{
    Q_OBJECT
public:
    explicit Script();

    struct Command {
        Command( const QString &name
                 , const QString &method
                 , const QString &description )
            : mName( name )
            , mMethod( method )
            , mDescription( description )
        {}

        QString mName;
        QString mMethod;
        QString mDescription;
    };

    bool Evaluate( QString input );
    void AddGlobal( const QString &name, QObject *obj );
    void AddCommand( const QString &name, const QString &method
                     , const QString &description );

    static bool IsVerbose(){ return mVerbose; }
    static void SetVerbose( bool verbose ){ mVerbose = verbose; }

public slots:
    void Help();

private:
    QScriptEngine mScriptEngine;

    QString ProcessInput( const QString &str );
    QString ReplaceArguments(const QString &cmd , QStringList args);
    QMap< QString, Command* > mCommandList;
    static bool mVerbose;

};

#endif // SCRIPT_H
