#include "keyhandler.h"
#include "maniaplayer.h"
#include <windows.h>
#include <QThread>

//
KeyHandler::KeyHandler( QO* p ) : QO{ p } {}

//
void KeyHandler::runner( void )
{
    QThread*     mania_player_t = nullptr;
    ManiaPlayer* mania_player   = nullptr;

    forever
    {
        while ( true ) {
            if ( mania_ready && ( GetAsyncKeyState( 'H' ) & 0x8000 ) ){
                break;
            }
            QThread::msleep( 40 );
        }

        emit begin_play( mfile );

        // Mania Player :
        mania_player_t = new QThread();
        mania_player   = new ManiaPlayer( {83, 68, 70, 71, 75, 76, 186 }, mfile );

        mania_player->moveToThread( mania_player_t );
        connect(
            mania_player_t, &QThread::started,
            mania_player,   &ManiaPlayer::runner );
        connect(
            this,           &KeyHandler::end_play,
            mania_player,   &ManiaPlayer::stop_runner, Qt::DirectConnection );
        connect( mania_player_t, &QThread::finished, this, [this]( void ){
            this->mania_ready = false;
        } );

        mania_player_t->start();

        //
        while ( true ) {
            if ( GetAsyncKeyState( 'J' ) & 0x8000 ) {
                break;
            }
            if ( !mania_ready ) {
                break;
            }
            QThread::msleep( 100 );
        }

        //
        emit end_play();
        mania_player_t->quit();
        mania_player_t->wait();
        mania_player_t = nullptr;
    }
}

//
void KeyHandler::mania_change( const bool& mania_ready, CQS& mfile ) {
    this->mania_ready = mania_ready;
    this->mfile       = mfile;
}
