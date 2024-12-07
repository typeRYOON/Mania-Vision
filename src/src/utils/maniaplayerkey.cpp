#include "maniaplayerkey.h"
#include <windows.h>
#include <chrono>
#include <thread>

//
ManiaPlayerKey::ManiaPlayerKey(
    const QVector<QPair<quint32, quint32>>& hm,
    const quint32& sd,
    const quint8& vk,
    QO* p
    ) : hit_map( hm ), start_delay( sd ), vk( vk), QO{ p } {}

//
void ManiaPlayerKey::stop_runner( void ) { cont = false; }

//
void ManiaPlayerKey::delay( const qint16& shift ) { this->shift += shift; }

//
void ManiaPlayerKey::runner( void )
{
    keybd_event( vk, 0, KEYEVENTF_KEYUP, 0 );
    auto start_time = std::chrono::high_resolution_clock::now();
    auto expected   = start_time + std::chrono::milliseconds( start_delay );
    if ( start_delay ) { std::this_thread::sleep_until( expected ); }

    start_time = expected;
    for ( const auto& pair : hit_map )
    {
        if ( !cont ) { break; }
        keybd_event( vk, 0, 0, 0 );

        std::this_thread::sleep_for( std::chrono::milliseconds( pair.first ) );
        keybd_event( vk, 0, KEYEVENTF_KEYUP, 0 );

        expected   = start_time + std::chrono::milliseconds( pair.second + pair.first + shift );
        start_time = expected, shift = 0;
        std::this_thread::sleep_until( expected );
    }
}
