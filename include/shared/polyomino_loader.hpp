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

    // Reads the binary file and returns a buffer the game can use
    // Using std::vector<char> ensures the memory stays alive
    std::vector<char> LoadBinaryFile(const std::string& filepath);

    // Load and parse binary file to extract canonical IDs
    std::set<int32_t> LoadCanonicalIds(const std::string& filepath);

    // Load and parse binary file to extract rotation mappings
    std::map<int32_t, int32_t> LoadRotations(const std::string& filepath);

    // Load and parse binary file to extract reflection mappings
    std::map<int32_t, int32_t> LoadReflections(const std::string& filepath);

    // Load and parse binary file to extract transformation mappings (any ID -> canonical ID)
    std::map<int32_t, int32_t> LoadTransformations(const std::string& filepath);
}

#endif // POLYOMINO_LOADER_HPP