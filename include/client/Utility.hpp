#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "shared/Team.hpp"
#include "Config.hpp"

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"

namespace Utility
{
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

    inline sf::IntRect getRect(Team team) 
    {
        int tileSize = static_cast<int>(Config::TileSize);
        sf::Vector2i size = {tileSize, tileSize};

        switch(team) 
        {
            case Team::Blue:   
                return sf::IntRect({0, 0}, size);
            case Team::Yellow: 
                return sf::IntRect({tileSize, 0}, size);
            case Team::Red:    
                return sf::IntRect({0, tileSize}, size); 
            case Team::Green:  
                return sf::IntRect({tileSize, tileSize}, size);
            default:           
                return sf::IntRect({0, 0}, {0, 0});
        }
    }
}

#endif
