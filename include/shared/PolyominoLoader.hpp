#ifndef POLYOMINO_LOADER_HPP
#define POLYOMINO_LOADER_HPP

#include "shared/PolyominoUtil.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>

namespace Blokus 
{
    // Saves the raw definitions into the FlatBuffer binary format
    void saveToBinary(const std::string& filepath, const Blokus::PolyominoDefinition& pieces);

    // Validates the binary file exists and can be read
    void validateBinaryFile(const std::string& filepath);

    // Load entire piece library from binary file
    PolyominoDefinition loadPieceLibrary(const std::string& filepath);

    std::vector<char> loadBinaryFile(const std::string& filepath);
}

#endif // POLYOMINO_LOADER_HPP