#include "game.hpp"

#include <iostream>


int main(int argc, char** argv)
{
    // Load pre-game data using where the pieces are stored
    std::string piece_path = std::string(PIECES_PATH);
    
    Game game;
    game.Run();

    return 0;
}