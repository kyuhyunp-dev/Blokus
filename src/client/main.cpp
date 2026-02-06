#include "Game.hpp"

#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Resource/Path.hpp"
#include <iostream>



int main(int argc, char** argv)
{
    initializeResourceRoot(argv[0]);

    // Load pre-game data using where the pieces are stored
    std::string piecePath = getAssetPath("shared/pieces.bin");
    // TODO: use piecePath

    FontHolder fonts;
	std::string fontFilename = getAssetPath("client/fonts/sansation.ttf");
    fonts.open(Fonts::ID::Sansation, fontFilename);

    TextureHolder textures;
    std::string tileFilename = getAssetPath("client/textures/tiles.png");
    textures.load(Textures::ID::Tiles, tileFilename);
    
    Game game(fonts, textures);
    game.run();

    return 0;
}