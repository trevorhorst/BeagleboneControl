#include "script.h"


bool Script::mVerbose = true;

/** ****************************************************************************
 * \brief   Constructor
 * ****************************************************************************/
Script::Script()
    : QObject( NULL)
{
    // setObjectName( "Script" );

    // Addinthe script to itself causes some serious issues.
    AddGlobal( "script", this );
}

/** ****************************************************************************
 * \brief   Exposes a qobject to the script engine
 * ****************************************************************************/
void Script::AddGlobal(const QString &name, QObject *obj)
{
    DEBUG( QString( "Adding global %0").arg( name ) );
    // Passing QtOwnership means that the script engine won't explicitly
    // delete the associated object
    QScriptValue val( mScriptEngine.newQObject(
                          obj, QScriptEngine::QtOwnership ) );
    mScriptEngine.globalObject().setProperty( name, val );
}

/** ****************************************************************************
 * \brief   Matches a script command with a method call and description
 * \param   name    Name of the command
 * \param   method  Script method to call
 * \param   description Description of the command
 * ****************************************************************************/
void Script::AddCommand( const QString &name, const QString &method
                         , const QString &description )
{
    QString key = name.toLower();
    if( mCommandList.contains( key ) ){
        // Command exists already, do nothing
    } else {
        Command *newCmd = new Command( name, method, description );
        mCommandList.insert( key, newCmd );
    }
}

/** ****************************************************************************
 * \brief   Evaluates a command using the QScriptEngine
 * ****************************************************************************/
bool Script::Evaluate( QString input )
{
    QString cmd = ProcessInput( input );
    // qDebug( qPrintable( input ) );

    // Evaluate and get the result
    QScriptValue result = mScriptEngine.evaluate( cmd );

    // Check for errors in the evaluation
    if( mScriptEngine.hasUncaughtException() ){
        QString error;
        error.sprintf( "%s", qPrintable(
                           mScriptEngine.uncaughtException().toString() ) );
        return false;
    }

    if( !result.isUndefined() && !result.isUndefined() )
        qDebug( "Result: %s", qPrintable( result.toString() ) );

    return true;
}

/** ****************************************************************************
 * \brief   Tokenize input and process as command and parameters
 * \param   input   Input string to process
 * ****************************************************************************/
QString Script::ProcessInput( const QString &input )
{
    // Split input into command and parameters
    QStringList tokenized = input.split(
                QRegExp("\\s"), QString::SkipEmptyParts );

    // Check that we actually have information to work with
    if( tokenized.count() > 0 ) {
        // The first parameter of our input should be the command
        if( mCommandList.contains( tokenized[ 0 ] ) ) {
            Command *cmd = mCommandList.value( tokenized[ 0 ] );
            QString toEval = ReplaceArguments( cmd->mMethod, tokenized );
            return toEval;
        }
    }

    // Just return the original string
    return input;
}

/** ****************************************************************************
 * \brief   Replace format specifiers with actual arguments
 * \param   input   Input string to process
 * ****************************************************************************/
QString Script::ReplaceArguments( const QString &cmd, QStringList args )
{
    QString completeCmd = cmd;
    if( args.count() > 0) {
        // Search for format specifiers
        while( completeCmd.contains( "%" ) ) {
            QString param;
            // Remove arugments that have been accounted for
            if( args.count() > 1 )
                param = args.takeAt( 1 );
            // Replace specifiers with an argument
            completeCmd = completeCmd.arg( param );
        }
    }
    return completeCmd;
}

void Script::Help()
{
    QMap< QString, Command* >::const_iterator iter;
    for( iter = mCommandList.begin(); iter != mCommandList.end(); iter++ ) {
        qDebug( "%s", qPrintable( iter.value()->mName ) );
    }
}
