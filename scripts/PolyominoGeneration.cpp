
#include "polyomino_generated.h"

#include "shared/PolyominoUtil.hpp"
#include "shared/PolyominoLoader.hpp"
#include <iostream>

int main(int argc, char** argv) 
{
    if (argc < 2) 
    { 
        std::cerr << "Usage: " << argv[0] << " <output_path>" << std::endl;
        return 1;
    }

    Blokus::PolyominoDefinition rawData = Blokus::PolyominoGenerator::getData();
    std::string outputPath = argv[1];

    Blokus::saveToBinary(outputPath, rawData);
    
    return 0;
}
