#include "game.hpp"
#include "pregame.hpp"

#include <iostream>


int main(int argc, char** argv)
{
    // Load pre-game data using where the pieces are stored
    std::string piece_path = std::string(PIECES_PATH);
    PreGame pregame(piece_path);
    
    Game game;
    game.Run();

    return 0;
}