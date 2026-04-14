#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <SFML/System/Vector2.hpp>
#include "shared/Constants.hpp"
#include "shared/Team.hpp"

#include "SFML/Graphics/Color.hpp"


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

   
    inline sf::Color getShadowColor(Team team) 
    {
        sf::Color color;
        switch (team) 
        {   
            case Team::None:
            {
                color = sf::Color(128, 128, 128);
                break;
            }
            case Team::Red:
            {
                color = sf::Color(255, 75, 51);
                break;
            }
            case Team::Blue:
            {
                color = sf::Color(46, 154, 255);
                break;
            }
            case Team::Yellow:
            {
                color = sf::Color(255, 215, 0);
                break;
            }
            case Team::Green:
            {
                color = sf::Color(73, 209, 27);
                break;
            }
            default:
            {
                assert(false && "Invalid Team!");
            }
        }

        color.a = 120; 
        return color;
    }
}

#endif