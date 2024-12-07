/* maniaplayer.h */
/* ---------------------------------------------------------------------------------------
 >> University of Illinois at Chicago - CS 415, Fall 2024
 >> --------------------------------------------------------------------------------------
 >> File    :: maniaplayer.h :: Mania-Vision: Mania HitMap Player
 >> Course  :: CS 415 (42844), FA24
 >> Author  :: Ryan Magdaleno
 >> UIN/nID :: 668523658 (rmagd2)
 >> System  :: Windows 10 w/ Mingw-w64
 - -                               - -
 >> File overview ::
 >> Header file, check implementation file for detailed explanations.
--------------------------------------------------------------------------------------- */
// Preprocessor directives ::
#ifndef MANIAPLAYER_H
#define MANIAPLAYER_H

#include "hitmap.h"

namespace maniaplayer {
    void play_hitmap( const HitMap& );
    void key_player(
        const std::vector<std::pair<uint32_t, uint32_t>>&&,
        const uint32_t,
        const uint8_t
    );
}

#endif // MANIAPLAYER_H :: - -                                                         - -
