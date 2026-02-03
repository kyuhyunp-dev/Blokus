#ifndef POLYOMINO_LOADER_HPP
#define POLYOMINO_LOADER_HPP

#include "shared/polyomino_util.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>

namespace Blokus 
{
    // Saves the raw definitions into the FlatBuffer binary format
    void SaveToBinary(const std::string& filepath, const Blokus::PolyominoDefinition& pieces);

    // Validates the binary file exists and can be read
    void ValidateBinaryFile(const std::string& filepath);

    // Load entire piece library from binary file
    PolyominoDefinition LoadPieceLibrary(const std::string& filepath);

    std::vector<char> LoadBinaryFile(const std::string& filepath);
}

#endif // POLYOMINO_LOADER_HPP