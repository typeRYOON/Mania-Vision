/* maniaparser.h */
/* ---------------------------------------------------------------------------------------
 >> University of Illinois at Chicago - CS 415, Fall 2024
 >> --------------------------------------------------------------------------------------
 >> File    :: maniaparser.h :: Mania-Vision: Mania File Parser
 >> Course  :: CS 415 (42844), FA24
 >> Author  :: Ryan Magdaleno
 >> UIN/nID :: 668523658 (rmagd2)
 >> System  :: Windows 10 w/ Mingw-w64
 - -                               - -
 >> File overview ::
 >> Header file, check implementation file for detailed explanations.
--------------------------------------------------------------------------------------- */
// Preprocessor directives ::
#ifndef MANIAPARSER_H
#define MANIAPARSER_H

#include "hitmap.h"

namespace maniaparser {
    HitMap hitmap_from_file( const std::string& );
    void   parse_hitframes( HitMap&, std::ifstream& );
}

#endif // MANIAPARSER_H :: - -                                                         - -
