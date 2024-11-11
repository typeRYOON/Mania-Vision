#include "hitmap.h"

namespace maniaplayer {
    void play_hitmap( const HitMap& );
    void key_player(
        const std::vector<std::pair<uint32_t, uint32_t>>&&,
        const uint32_t,
        const uint8_t
    );
}
