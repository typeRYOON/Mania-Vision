#ifndef MANIAPARSER_H
#define MANIAPARSER_H
#include "hitmap.h"

//
namespace maniaparser {
    HitMap hitmap_from_file( const std::string& );
    void   parse_hitframes( HitMap&, std::ifstream& );
}

#endif // MANIAPARSER_H :: - -                                                         - -
