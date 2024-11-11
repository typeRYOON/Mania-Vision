#ifndef HITMAP_H
#define HITMAP_H

#include <filesystem>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>

// 
struct HitMap {
    uint32_t                fps;
    std::string             err = "";
    std::filesystem::path   original_file;
    std::array<uint8_t,  4> virtual_keys;
    std::array<uint32_t, 4> start_delays;
    std::array<std::vector<std::pair<uint32_t, uint32_t>>, 4> hitmap;
    HitMap( std::filesystem::path f ) : original_file( f ) {}
};

#endif // HITMAP_H :: - -                                                              - -