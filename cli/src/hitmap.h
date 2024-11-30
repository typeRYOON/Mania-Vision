/* hitmap.h */
/* ---------------------------------------------------------------------------------------
 >> University of Illinois at Chicago - CS 415, Fall 2024
 >> --------------------------------------------------------------------------------------
 >> File    :: hitmap.h :: Mania-Vision: HitMap Type
 >> Course  :: CS 415 (42844), FA24
 >> Author  :: Ryan Magdaleno
 >> UIN/nID :: 668523658 (rmagd2)
 >> System  :: Windows 10 w/ Mingw-w64
 - -                               - -
 >> File overview ::
 >> TODO
--------------------------------------------------------------------------------------- */
// Preprocessor directives ::
#ifndef HITMAP_H
#define HITMAP_H

#include <filesystem>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>

// HitMap struct type, mania file storage program type :: - -                          - -
struct HitMap {
    uint32_t                fps_numerator;
    uint32_t                fps_denominator;
    std::string             err = "";
    std::filesystem::path   original_file;
    std::array<uint8_t,  4> virtual_keys;
    std::array<uint32_t, 4> start_delays;
    std::array<std::vector<std::pair<uint32_t, uint32_t>>, 4> hitmap;
    HitMap( std::filesystem::path f ) : original_file( f ) {}
};

#endif // HITMAP_H :: - -                                                              - -
