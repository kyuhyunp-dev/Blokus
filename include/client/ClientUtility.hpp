#ifndef CLIENT_UTILITY_HPP
#define CLIENT_UTILITY_HPP

#include "shared/Team.hpp"
#include "ClientConfig.hpp"

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Text.hpp"

#include <cassert>


sf::Color getShadowColor(Team team);

sf::IntRect getRect(Team team);
    
void centerOrigin(sf::Text& text);

#endif
