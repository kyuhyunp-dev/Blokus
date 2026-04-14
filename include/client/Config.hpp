#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <SFML/System/Vector2.hpp>
#include "shared/Constants.hpp"


namespace Config {
    const unsigned int ScreenWidth = 1920;
    const unsigned int ScreenHeight = 1080;
    
    const sf::Vector2f VirtualRes = { 
        static_cast<float>(ScreenWidth), 
        static_cast<float>(ScreenHeight) 
    };

    // --- Board Geometry (12x12) ---
    const float GridSize = 40.f; 
    const float TileSize = 512.f;

    const float Padding = 100.f;
}

#endif