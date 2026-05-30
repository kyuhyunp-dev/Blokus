#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <SFML/System/Vector2.hpp>
#include "shared/SharedConfig.hpp"
#include "shared/Team.hpp"

#include "SFML/Graphics/Color.hpp"


namespace Config 
{
    inline constexpr unsigned int ScreenWidth = 1920;
    inline constexpr unsigned int ScreenHeight = 1080;
    
    inline constexpr sf::Vector2f VirtualRes = { 
        static_cast<float>(ScreenWidth), 
        static_cast<float>(ScreenHeight) 
    };

    // --- Board Geometry (12x12) ---
    inline constexpr float GridSize = 40.f; 
    inline constexpr float TileSize = 512.f;

    inline constexpr float Padding = 100.f;
}

#endif