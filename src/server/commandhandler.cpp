#include "commandhandler.h"

CommandHandler::CommandHandler( QObject *parent )
    : QObject( parent )
{

}

bool CommandHandler::AddCommand(QString type, Command *cmd)
{
    if( mCommandList.contains( type ) ) {
        qDebug( "CommandHandler: Command already exists" );
        return false;
    }

    mCommandList.insert( type, cmd );
    return true;
}
