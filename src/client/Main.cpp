#include "Application.hpp"

#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Path.hpp"
#include <iostream>



int main(int argc, char** argv)
{
    initializeResourceRoot(argv[0]);

    FontHolder fonts;
	std::string fontFilename = getAssetPath("client/fonts/sansation.ttf");
    fonts.load(Fonts::ID::Sansation, fontFilename);
    
    Application app(fonts);
    app.run();

    return 0;
}