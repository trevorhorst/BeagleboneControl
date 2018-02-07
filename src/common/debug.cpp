#include "debug.h"

Debug::Debug(QObject *parent) : QObject(parent)
{

}

void Debug::Init( Script *script )
{
    script->AddGlobal( "debug", this );
    script->AddCommand( "debug", "debug.Toggle('%0')"
                        , "Toggle an objects debug mode" );
}

void Debug::Add( QString name, bool ( *get )( void ), void ( *set )( bool ) )
{
    qDebug( "Adding %s", qPrintable( name ) );
    DebugItem *item = new DebugItem( name, get, set );
    mDebugList.insert( name, item );
}

void Debug::Toggle( QString name )
{
    // Check if the name is in our list of debug items
    if( mDebugList.contains( name ) ){
        DebugItem *item = mDebugList.value( name );
        bool verbose = item->mGet();
        if( verbose ) {
            qDebug( "Debug %s: Disabled", qPrintable( name ) );
        } else {
            qDebug( "Debug %s: Enabled", qPrintable( name ) );
        }
        item->mSet( !verbose );
    } else {
        QMap< QString, DebugItem* >::const_iterator iter;
        for( iter = mDebugList.begin(); iter != mDebugList.end(); iter++ ) {
            qDebug( "%s", qPrintable( iter.value()->mName ) );
        }
    }
}
