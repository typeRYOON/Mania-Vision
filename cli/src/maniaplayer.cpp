#include "maniaplayer.h"
#include <condition_variable>
#include <unordered_map>
#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

//
namespace maniaplayer
{
    std::unordered_map<std::thread::id, uint32_t> thread_shifts;
    std::atomic<bool>     terminate( false );
    std::mutex mtx;
    std::condition_variable cv;
    uint8_t active_threads = 0;

    //
    void key_player(
        const std::vector<std::pair<uint32_t, uint32_t>>&& hit_map,
        const uint32_t start_delay,
        const uint8_t  virtual_key )
    {
        keybd_event( virtual_key, 0, KEYEVENTF_KEYUP, 0 );
        
        auto start_time = std::chrono::high_resolution_clock::now();
        auto expected   = start_time + std::chrono::milliseconds( start_delay );
        if ( start_delay ) { std::this_thread::sleep_until( expected ); }
        
        const auto tid = std::this_thread::get_id();
        thread_shifts[tid] = 0;

        start_time = expected;
        for ( const auto& pair : hit_map )
        {
            if ( terminate ) { break; }
            keybd_event( virtual_key, 0, 0, 0 );

            std::this_thread::sleep_for( std::chrono::milliseconds( pair.first ) );
            keybd_event( virtual_key, 0, KEYEVENTF_KEYUP, 0 );

            expected = start_time + std::chrono::milliseconds(
                pair.second + pair.first + thread_shifts[tid]
            );
            thread_shifts[tid] = 0;
            start_time = expected;
            std::this_thread::sleep_until( expected );
        }
        keybd_event( virtual_key, 0, KEYEVENTF_KEYUP, 0 );
        std::lock_guard<std::mutex> lock( mtx );
        --active_threads;
        cv.notify_all();
    }

    //
    void play_hitmap( const HitMap& hit_map )
    {
        std::cout << "Press H to start.\n";
        while ( true ) {
            if ( GetAsyncKeyState( 'H' ) & 0x8000 ) { break; }
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }

        // Launch threads :
        std::vector<std::thread> threads;
        active_threads = 4;
        for ( uint8_t i = 0; i < 4; ++i )
        {
            threads.emplace_back(
                &key_player,
                std::move( hit_map.hitmap.at(i) ),
                hit_map.start_delays.at(i),
                hit_map.virtual_keys.at(i)
            );
        }

        // Check for J (term), 1 (-3ms shift), 2 (+3ms shift) :
        while ( true )
        {
            // Check if all threads have finished :
            {
                std::unique_lock<std::mutex> lock( mtx );
                if ( cv.wait_for( lock,
                                  std::chrono::milliseconds( 1000 ),
                                  [] { return active_threads == 0; } ) 
                ) { break; }
            }
            // Key monitors :
            if ( GetAsyncKeyState( 'J' ) & 0x8000 ) { terminate = true; break; }
            if ( GetAsyncKeyState( '1' ) & 0x8000 ) {
                for ( auto& [thread_id, shift] : thread_shifts ) { shift -= 3; }
            }
            else if ( GetAsyncKeyState( '2' ) & 0x8000 ) {
                for ( auto& [thread_id, shift] : thread_shifts ) { shift += 3; }
            }
        }

        // Join back threads :
        for ( std::thread& thread : threads ) {
            if ( thread.joinable() ) { thread.join(); }
        }
    }
}