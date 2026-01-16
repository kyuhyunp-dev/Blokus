#ifndef POLYOMINO_LOADER_HPP
#define POLYOMINO_LOADER_HPP

#include "shared/polyomino_util.hpp"

#include <string>
#include <vector>

namespace Blokus 
{
    // Saves the raw definitions into the FlatBuffer binary format
    void save_to_binary(const std::string& filepath, const std::vector<PolyominoDefinition>& pieces);

    // Reads the binary file and returns a buffer the game can use
    // Using std::vector<char> ensures the memory stays alive
    std::vector<char> load_binary_file(const std::string& filepath);

    // Additional parsing functions for corners, edges, ids, etc. can be added here
}

#endif // POLYOMINO_LOADER_HPP