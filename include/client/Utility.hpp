#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "shared/Team.hpp"
#include "Config.hpp"

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Text.hpp"

#include <cassert>


sf::Color getShadowColor(Team team);

sf::IntRect getRect(Team team);
    
void centerOrigin(sf::Text& text);

#endif
