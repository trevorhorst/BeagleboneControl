#ifndef DEBUG_H
#define DEBUG_H

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>

#include "script.h"

class Debug : public QObject
{
    Q_OBJECT
public:
    explicit Debug(QObject *parent = 0);


    struct DebugItem {
        DebugItem( QString name
                   , bool ( *get )( void )
                   , void ( *set )( bool ) )
            : mName( name )
            , mGet( get )
            , mSet( set )
        {}
        // Identifier
        QString mName;
        // Function pointer to the get
        bool ( *mGet )( void );
        // Function pointer to the set
        void ( *mSet )( bool );
    };

    void Init( Script *script );
    void Add( QString name, bool ( *get )( void ), void ( set )( bool ) );

signals:

public slots:
    void Toggle( QString name );

private:
    QMap< QString, DebugItem* > mDebugList;
};

#endif // DEBUG_H
