#ifndef POLYOMINO_UTIL_HPP
#define POLYOMINO_UTIL_HPP

#include <vector>
#include <set>

namespace Blokus
{
    struct PolyominoDefinition {
        int id;
    };

    std::vector<PolyominoDefinition> generate_polyomino_metadata();

}

#endif // POLYOMINO_UTIL_HPP