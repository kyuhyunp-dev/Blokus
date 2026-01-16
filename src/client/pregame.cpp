#include "pregame.hpp"

#include "polyomino_generated.h"
#include "shared/polyomino_loader.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>


PreGame::PreGame(const std::string& polyomino_path)
{
    std::vector<char> buffer = Blokus::load_binary_file(polyomino_path);
    auto piece_library = Blokus::Data::GetPieceLibrary(buffer.data());

    // Access the data
    auto pieces = piece_library->pieces(); 
    std::cout << "Successfully loaded " << pieces->size() << " pieces!" << std::endl;

    for (unsigned int i = 0; i < pieces->size(); ++i) 
    {
        auto piece = pieces->Get(i);
        std::cout << "Piece Index: " << i << " | ID: " << piece->id() << std::endl;
    }
}