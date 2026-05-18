#include "Utility.hpp"
#include <cmath>


sf::Color getShadowColor(Team team)
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
            break;
        }
    }

    color.a = 120; 
    return color;
}

sf::IntRect getRect(Team team) 
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

void centerOrigin(sf::Text& text)
{
    sf::Vector2f center = text.getLocalBounds().getCenter();
    text.setOrigin({ std::floor(center.x), std::floor(center.y) });
}

