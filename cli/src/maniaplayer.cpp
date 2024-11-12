/* maniaplayer.cpp */
/* ---------------------------------------------------------------------------------------
 >> University of Illinois at Chicago - CS 415, Fall 2024
 >> --------------------------------------------------------------------------------------
 >> File    :: maniaplayer.cpp :: Mania-Vision: Mania HitMap Player
 >> Course  :: CS 415 (42844), FA24
 >> Author  :: Ryan Magdaleno
 >> UIN/nID :: 668523658 (rmagd2)
 >> System  :: Windows 10 w/ Mingw-w64
 - -                               - -
 >> File overview ::
 >> TODO
--------------------------------------------------------------------------------------- */
// Preprocessor directives ::
#include "maniaplayer.h"

#include <condition_variable>
#include <unordered_map>
#include <windows.h>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

namespace maniaplayer
{   // Thread synchronization variables :: - -                                         - -
    std::unordered_map<std::thread::id, int32_t> shifts; // Shift applied to each thread.
    std::atomic<bool>       terminate( false );          // User end condition.
    std::mutex              mtx;                         // Mutual exclusion helper.
    std::condition_variable cv;                          // Thread condition blocker.
    uint8_t                 active_threads = 0;          // Organic end condition.

    // Thread key runner function, precisely play a given hit_map for a key :: - -     - -
    void key_player(
        const std::vector<std::pair<uint32_t, uint32_t>>&& hit_map,
        const uint32_t start_delay,
        const uint8_t  virtual_key )
    {
        // Release any held keys :
        keybd_event( virtual_key, 0, KEYEVENTF_KEYUP, 0 );

        // Using chrono, create precise times for the current thread to wait until :
        auto start_time = std::chrono::high_resolution_clock::now();
        auto expected   = start_time + std::chrono::milliseconds( start_delay );
        if ( start_delay ) { std::this_thread::sleep_until( expected ); }

        // Initialize unique shift for current thread :
        auto [it, _] = shifts.emplace( std::this_thread::get_id(), 0 );

        for ( const auto& hit : hit_map )
        {   // User terminated early :
            if ( terminate ) { break; }

            // Press key and hold for needed duration :
            keybd_event( virtual_key, 0, 0, 0 );
            std::this_thread::sleep_for( std::chrono::milliseconds( hit.first ) );

            // Release key and set up delay time :
            keybd_event( virtual_key, 0, KEYEVENTF_KEYUP, 0 );
            start_time = expected;
            expected   = start_time + std::chrono::milliseconds(
                hit.first + hit.second + it->second
            );

            // Sleep for delay time to next press, reset possible shift :
            std::this_thread::sleep_until( expected );
            it->second = 0;
        }
        // Thread clean up :
        keybd_event( virtual_key, 0, KEYEVENTF_KEYUP, 0 );
        std::lock_guard<std::mutex> lock( mtx );
        active_threads--;
        cv.notify_all();
    }

    // Thread controller function, launch and send cv signals to child threads :: - -  - -
    void play_hitmap( const HitMap& hit_map )
    {
        std::cout << "\n > Press H to start.";
        while ( true ) {
            if ( GetAsyncKeyState( 'J' ) & 0x8000 ) { return; } // Early return.
            if ( GetAsyncKeyState( 'H' ) & 0x8000 ) { break ; } // Start.
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }

        // Launch threads :
        std::vector<std::thread> threads;
        for ( uint8_t i = 0; i < 4; ++i )
        {
            threads.emplace_back(
                &key_player,
                std::move( hit_map.hitmap.at(i) ),
                hit_map.start_delays.at(i),
                hit_map.virtual_keys.at(i)
            );
        }
        active_threads = threads.size();

        // Check for J (term), 1 ( -3ms shift ), 2 ( +3ms shift ) :
        while ( true )
        {   // Check if all threads have finished :
            {   // Limit lock lifespan :
                std::unique_lock<std::mutex> lock( mtx );
                if ( cv.wait_for( lock,
                                  std::chrono::milliseconds( 1000 ),
                                  [] { return active_threads == 0; } ) 
                ) { break; }
            }
            // Key monitors :
            if ( GetAsyncKeyState( 'J' ) & 0x8000 ) { terminate = true; }
            else if ( GetAsyncKeyState( '1' ) & 0x8000 ) {
                for ( auto& [_, shift] : shifts ) { shift -= 3; }
            }
            else if ( GetAsyncKeyState( '2' ) & 0x8000 ) {
                for ( auto& [_, shift] : shifts ) { shift += 3; }
            }
        }

        // Join back threads :
        for ( std::thread& thread : threads ) {
            if ( thread.joinable() ) { thread.join(); }
        }
    }
}   // :: - -                                                                          - -
