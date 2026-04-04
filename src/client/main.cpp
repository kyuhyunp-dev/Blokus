#include "Game.hpp"

#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Path.hpp"
#include <iostream>



int main(int argc, char** argv)
{
    initializeResourceRoot(argv[0]);

    // Load pre-game data using where the pieces are stored
    std::string piecePath = getAssetPath("shared/pieces.bin");
    // TODO: use piecePath

    FontHolder fonts;
	std::string fontFilename = getAssetPath("client/fonts/sansation.ttf");
    fonts.load(Fonts::ID::Sansation, fontFilename);
    
    Game game(fonts);
    game.run();

    return 0;
}