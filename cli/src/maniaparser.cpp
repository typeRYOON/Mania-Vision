#include "maniaparser.h"
#include <fstream>
#include <sstream>

//
namespace maniaparser
{
    //
    void parse_hitframes( HitMap& hm, std::ifstream& instream )
    {
        uint32_t time = 1;
        std::string line;
        std::array<uint32_t, 4> hold_map = { 0, 0, 0, 0 };
        std::array<std::vector<std::pair<uint32_t, uint32_t>>, 4> inter;
        for ( uint8_t i = 0; i < 4; ++i ) {
            inter[i] = std::vector<std::pair<uint32_t, uint32_t>>();
        }

        // Stage 1 - Get press start/end times :
        while ( std::getline( instream, line ) )
        {
            for ( uint8_t i = 1, j = 0; i != 16; i <<= 1, j++ )
            {   
                // In press :
                if ( std::stoi( line ) & i )
                {   // New press :
                    if ( !hold_map.at(j) ) { hold_map[j] = time; }
                }

                // No press :
                else
                {   // Released press :
                    if ( hold_map.at(j) ) {
                        inter[j].emplace_back(
                            std::make_pair( hold_map.at(j), time )
                        );
                        hold_map[j] = 0;
                    }
                }
            }
            time++;
        }
        instream.close();

        // Stage 1.5a - Taper off loose ends ( player holds until end ) :
        for ( uint8_t i = 0; i < 4; ++i )
        {
            if ( hold_map.at(i) ) {
                inter[i].emplace_back(
                    std::make_pair( hold_map.at(i), time )
                );
            }
            if ( inter.at(i).empty() ) {
                hm.err = "Empty column.";
                return;
            }
            // Reserve space in final vector :
            hm.hitmap[i] = std::vector<std::pair<uint32_t, uint32_t>>();
            hm.hitmap[i].reserve( inter.at(i).size() );
        }

        // Stage 1.5b - Create start delays :
        uint32_t earliest = UINT32_MAX;
        uint32_t htime_f, dtime_f, htime_ms, dtime_ms, carry_ms, n;
        const uint32_t frame_ms      = 1000 / hm.fps;
        const uint32_t sec_remain_ms = 1000 % hm.fps;

        for ( uint8_t i = 0; i < 4; ++i ) {
            if ( inter.at(i).at(0).first < earliest ) {
                earliest = inter.at(i).at(0).first;
            }
        }
        for ( uint8_t i = 0; i < 4; ++i ) {
            dtime_f  = inter.at(i).at(0).first - earliest;
            hm.start_delays[i] = dtime_f * frame_ms + ( dtime_f * sec_remain_ms ) / hm.fps;
        }

        // Stage 2 - Transform hold times and release times to ms using fps :
        for ( uint8_t i = 0; i < 4; ++i )
        {
            carry_ms = 0;
            n = inter.at(i).size() - 1;

            for ( uint32_t j = 0; j < n; ++j )
            {   // Frame duration calculations :
                htime_f = inter.at(i).at(j).second - inter.at(i).at(j).first;
                dtime_f = inter.at(i).at(j + 1).first - inter.at(i).at(j).first - htime_f;

                // Hold time - frames to ms :
                htime_ms = htime_f * frame_ms + ( htime_f * sec_remain_ms + carry_ms ) / hm.fps;
                carry_ms = ( htime_f * sec_remain_ms + carry_ms ) % hm.fps;

                // Delay time - frames to ms :
                dtime_ms = dtime_f * frame_ms + ( dtime_f * sec_remain_ms + carry_ms ) / hm.fps;
                carry_ms = ( dtime_f * sec_remain_ms + carry_ms ) % hm.fps;

                hm.hitmap[i].emplace_back( std::make_pair( htime_ms, dtime_ms ) );
            }
            htime_f  = inter.at(i).at(n).second - inter.at(i).at(n).first;
            htime_ms = htime_f * frame_ms + ( htime_f * sec_remain_ms + carry_ms ) / hm.fps;
            hm.hitmap[i].emplace_back( std::make_pair( htime_ms, 0 ) );
        }
    }

    //
    HitMap hitmap_from_file( const std::string& file_path )
    {
        std::ifstream file( file_path );
        HitMap ret( file_path );

        // Check if file exists :
        if ( !file.is_open() ) {
            ret.err = file_path + " does not exist.";
            return ret;
        }

        // Check if mania header exists :
        std::string header;
        if ( !std::getline( file, header ) ) {
            ret.err = "Empty mania header.";
            file.close();
            return ret;
        }

        // Parse mania header :
        std::stringstream header_stream( header );
        uint32_t vk1, vk2, vk3, vk4;
        
        if ( !( header_stream >> ret.fps >> vk1 >> vk2 >> vk3 >> vk4 ) ) {
            ret.err = "Header format invalid.";
            file.close();
            return ret;
        }
        ret.virtual_keys = {
            static_cast<uint8_t>( vk1 ),
            static_cast<uint8_t>( vk2 ),
            static_cast<uint8_t>( vk3 ),
            static_cast<uint8_t>( vk4 )
        };
        parse_hitframes( ret, file );
        return ret;
    }
}