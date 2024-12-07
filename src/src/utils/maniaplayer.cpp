#include "maniaplayer.h"
#include "maniaplayerkey.h"
#include <windows.h>
#include <QThread>
#include <QFile>

//
ManiaPlayer::ManiaPlayer( const QVector<quint8>& vks, CQS& mfile, QO* p )
    : vks( vks ), mfile( mfile ), QO{ p } {}

//
void ManiaPlayer::runner( void )
{
    read_mfile();

    QVector<ManiaPlayerKey*> obj    ( key_n, nullptr );
    QVector<QThread*>        threads( key_n, nullptr );

    for ( quint8 i = 0; i < key_n; ++i )
    {
        threads[i] = new QThread();
        obj[i]     = new ManiaPlayerKey(
            hit_chart.at(i),
            hit_chart.back().at(i).first,
            vks.at(i)
        );
        obj.at(i)->moveToThread( threads.at(i) );

        connect(
            threads.at(i), &QThread       ::started,
            obj.at(i),     &ManiaPlayerKey::runner,      Qt::DirectConnection );
        connect(
            threads.at(i), &QThread       ::finished,
            obj.at(i),     &ManiaPlayerKey::deleteLater, Qt::DirectConnection );
        connect(
            threads.at(i), &QThread       ::finished,
            threads.at(i), &QThread       ::deleteLater, Qt::DirectConnection );
        connect(
            this,          &ManiaPlayer   ::delay,
            obj.at(i),     &ManiaPlayerKey::delay,       Qt::DirectConnection);
        connect(
            this,          &ManiaPlayer   ::finished_mania,
            obj.at(i),     &ManiaPlayerKey::stop_runner, Qt::DirectConnection );
    }
    for ( quint8 i = 0; i < key_n; ++i ) { threads.at(i)->start(); }
    while ( cont )
    {
        if      ( GetAsyncKeyState( '1' ) & 0x8000 ) { qDebug() << 3;  emit delay( -3 ); }
        else if ( GetAsyncKeyState( '2' ) & 0x8000 ) { qDebug() << -3; emit delay( +3 ); }
        QThread::msleep( 250 );
    }
    emit finished_mania();
    for ( quint8 i = 0; i < key_n; ++i ) {
        threads.at(i)->quit();
        threads.at(i)->wait();
        delete threads.at(i);
    }
}

//
void ManiaPlayer::stop_runner( void ) {
    cont = false;
}

// TODO, read encoding file "mfile"  into hit_chart.
void ManiaPlayer::read_mfile( void )
{
    qDebug() << this->mfile;
}

//
qint32 ManiaPlayer::set_start_delays()
{
    quint32 earliest = UINT32_MAX;

    // Find the earliest time value among the first notes in each column :: - -        - -
    for ( quint8 i = 0; i < key_n; ++i ) {
        if ( hit_chart.at(i).isEmpty() ) { return -1; }
        if ( hit_chart.at(i).at(0).first < earliest ) {
            earliest = hit_chart.at(i).at(0).first;
        }
    }
    // Adjust the time values of the first notes in each column :: - -                 - -
    for ( quint8 i = 0; i < key_n; ++i ) {
        hit_chart[key_n] << qMakePair( hit_chart.at(i).at(0).first - earliest, 0 );
    }
    return earliest;
}
