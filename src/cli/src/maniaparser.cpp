/* maniaparser.cpp */
/* ---------------------------------------------------------------------------------------
 >> University of Illinois at Chicago - CS 415, Fall 2024
 >> --------------------------------------------------------------------------------------
 >> File    :: maniaparser.cpp :: Mania-Vision: Mania File parser
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

#include <fstream>
#include <sstream>

namespace maniaparser
{
    // Parse mania file's frames, get column timeline, convert frame time to ms time.  - -
    void parse_hitframes( HitMap& hm, std::ifstream& instream )
    {
        std::array<uint32_t, 4> start_times = { 0, 0, 0, 0 };
        std::string             line;
        uint32_t                line_data;
        uint32_t                time = 1;

        // Timeline is an array that has each column's press start and end time as a pair.
        std::array<std::vector<std::pair<uint32_t, uint32_t>>, 4> timeline;
        for ( uint8_t i = 0; i < 4; ++i ) {
            timeline[i] = std::vector<std::pair<uint32_t, uint32_t>>();
        }

        // Stage 1 - Get press start/end times :
        while ( std::getline( instream, line ) )
        {   // Attempt line string to integer conversion :
            try { line_data = std::stoi( line ); }
            catch ( ... ) {
                instream.close();
                hm.err = "Bad line ("
                         + std::to_string( time )
                         + "): \"" + line + "\".";
                return;
            }

            // Check each column bit, get start/end time of each press :
            for ( uint8_t i = 0, column_bit = 1; i < 4; ++i, column_bit <<= 1 )
            {   // In press :
                if ( line_data & column_bit )
                {   // New press :
                    if ( !start_times.at(i) ) { start_times[i] = time; }
                }
                // No press :
                else
                {   // Released press :
                    if ( start_times.at(i) ) {
                        timeline[i].emplace_back(
                            std::make_pair( start_times.at(i), time )
                        );
                        start_times[i] = 0;
                    }
                }
            }
            time++;
        }
        instream.close();

        // Stage 1.5a - Taper off loose ends ( player holds until end ) :
        for ( uint8_t i = 0; i < 4; ++i )
        {
            if ( start_times.at(i) ) {
                timeline[i].emplace_back(
                    std::make_pair( start_times.at(i), time )
                );
            }
            if ( timeline.at(i).empty() ) {
                hm.err = "Empty column.";
                return;
            }
            // Reserve space in final vector :
            hm.hitmap[i] = std::vector<std::pair<uint32_t, uint32_t>>();
            hm.hitmap[i].reserve( timeline.at(i).size() );
        }

        // Frame to ms conversion variables :
        const uint64_t frame_ms_numerator   = 1000 * hm.fps_denominator;
        const uint64_t frame_ms_denominator = hm.fps_numerator;
        uint64_t total_htime, total_dtime;
        uint32_t htime_f, dtime_f;
        uint64_t carry = 0;

        // Stage 1.5b - Create start delays :
        uint32_t earliest = UINT32_MAX;
        for ( uint8_t i = 0; i < 4; ++i ) {
            if ( timeline.at(i).at(0).first < earliest ) {
                earliest = timeline.at(i).at(0).first;
            }
        }
        for ( uint8_t i = 0; i < 4; ++i ) {
            total_dtime = ( timeline.at(i).at(0).first - earliest ) * frame_ms_numerator;
            hm.start_delays[i] = total_dtime / frame_ms_denominator;
        }

        // Stage 2 - Transform hold times and delay times to ms using fps :
        for ( size_t i = 0; i < 4; ++i )
        {   // Tterate to n-1 element, reset carry for column:
            size_t n = timeline.at(i).size() - 1;
            carry    = 0;

            for ( size_t j = 0; j < n; ++j )
            {   // Frame duration calculations :
                htime_f = timeline.at(i).at(j).second    - timeline.at(i).at(j).first;
                dtime_f = timeline.at(i).at(j + 1).first - timeline.at(i).at(j).second;

                // Hold time - frames to ms :
                total_htime = htime_f * frame_ms_numerator + carry;
                carry       = total_htime % frame_ms_denominator;

                // Delay time - frames to ms :
                total_dtime = dtime_f * frame_ms_numerator + carry;
                carry       = total_dtime % frame_ms_denominator;

                // Hold/delay time entry:
                hm.hitmap[i].emplace_back(
                    std::make_pair(
                        total_htime / frame_ms_denominator, 
                        total_dtime / frame_ms_denominator
                    )
                );
            }
            // nth hold/delay time entry:
            htime_f     = timeline.at(i).back().second - timeline.at(i).back().first;
            total_htime = htime_f * frame_ms_numerator + carry;
            hm.hitmap[i].emplace_back(
                std::make_pair(
                    total_htime / frame_ms_denominator,
                    0
                )
            );
        }
    }

    // Open file and retrieve header info, continue onto parse_hitframes func :: - -   - -
    HitMap hitmap_from_file( const std::string& file_path )
    {
        HitMap hm( file_path );

        // Argument check :
        if ( file_path.empty() ) {
            hm.err = "Must pass a mania file argument.";
            return hm;
        }

        // Check if file exists :
        std::ifstream file( file_path );
        if ( !file.is_open() ) {
            hm.err = file_path + " does not exist.";
            return hm;
        }

        // Check if mania header exists :
        std::string header;
        if ( !std::getline( file, header ) ) {
            hm.err = "Empty mania header.";
            file.close();
            return hm;
        }

        // Parse mania header :
        std::stringstream header_stream( header );
        uint32_t vk1, vk2, vk3, vk4;

        // Must be of format: "FPS_NUM FPS_DEN VK1 VK2 VK3 VK4"
        if ( !( header_stream >> hm.fps_numerator >> hm.fps_denominator
            >> vk1 >> vk2 >> vk3 >> vk4 ) )
        {
            hm.err = "Header format invalid.";
            file.close();
            return hm;
        }
        hm.virtual_keys = {
            static_cast<uint8_t>( vk1 ),
            static_cast<uint8_t>( vk2 ),
            static_cast<uint8_t>( vk3 ),
            static_cast<uint8_t>( vk4 )
        };
        parse_hitframes( hm, file );
        return hm;
    }
}   // :: - -                                                                          - -
