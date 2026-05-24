#include "Application.hpp"

#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Path.hpp"
#include <iostream>



int main(int argc, char** argv)
{
    initializeResourceRoot(argv[0]);

    TextureHolder textures;    
    FontHolder fonts;
    fonts.load(Fonts::ID::Sansation, "client/fonts/sansation.ttf");
    
    PolyominoGenerator libraryGenerator;
    
    Application app(textures, fonts, libraryGenerator.getData());
    app.run();

    return 0;
}