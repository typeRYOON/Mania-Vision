#include "maniaparser.h"
#include "maniaplayer.h"

//
int32_t main( int32_t argc, char* argv[] )
{
    if ( argc < 2 ) { return -1; }
    const HitMap hit_map = maniaparser::hitmap_from_file( argv[1] );

    if ( !hit_map.err.empty() )
    {
        std::cerr << "\n > Error: " << hit_map.err << std::endl;
        return -1;
    }

    maniaplayer::play_hitmap( hit_map );
    return 0;
}
