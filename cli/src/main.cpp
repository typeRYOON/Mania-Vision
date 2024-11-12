/* main.cpp */
/* ---------------------------------------------------------------------------------------
 >> University of Illinois at Chicago - CS 415, Fall 2024
 >> --------------------------------------------------------------------------------------
 >> File    :: main.cpp :: Mania-Vision: Program Entrypoint
 >> Course  :: CS 415 (42844), FA24
 >> Author  :: Ryan Magdaleno
 >> UIN/nID :: 668523658 (rmagd2)
 >> System  :: Windows 10 w/ Mingw-w64
 - -                               - -
 >> File overview ::
 >> TODO
--------------------------------------------------------------------------------------- */
// Preprocessor directives ::
#include "maniaparser.h"
#include "maniaplayer.h"

// Program entrypoint :: - -                                                           - -
int32_t main( int32_t argc, char* argv[] )
{   // Needs a file command line argument :
    if ( argc < 2 ) { 
        std::cerr << "\n > Error: Must pass a mania file argument."  << std::endl;
        return -1;
    }

    // Create HitMap object from passed file :
    const HitMap hit_map = maniaparser::hitmap_from_file( argv[1] );
    if ( !hit_map.err.empty() ) {
        std::cerr << "\n > Error: " << hit_map.err << std::endl;
        return -1;
    }

    // Run HitMap object's hitmap vector :
    maniaplayer::play_hitmap( hit_map );
    return 0;
}   // :: - -                                                                          - -
