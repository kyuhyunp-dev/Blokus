#include <vector>
#include "shared/polyomino_util.hpp"


namespace Blokus
{
    std::vector<PolyominoDefinition> generate_polyomino_metadata()
    {
        std::vector<PolyominoDefinition> definitions;
        // Dummy implementation for illustration
        for (int i = 0; i < 10; ++i) {
            PolyominoDefinition def;
            def.id = i;
            definitions.push_back(def);
        }
        return definitions;
    }
}
