
#include "polyomino_generated.h"

#include "shared/polyomino_util.hpp"
#include "shared/polyomino_loader.hpp"
#include <iostream>

int main(int argc, char** argv) 
{
    if (argc < 2) 
    { 
        std::cerr << "Usage: " << argv[0] << " <output_path>" << std::endl;
        return 1;
    }

    std::vector<Blokus::PolyominoDefinition> raw_data = Blokus::generate_polyomino_metadata();
    std::string output_path = argv[1];

    Blokus::save_to_binary(output_path, raw_data);
    
    return 0;
}
