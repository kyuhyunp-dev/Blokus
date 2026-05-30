#ifndef SHARED_CONFIG_HPP 
#define SHARED_CONFIG_HPP

#include "SFML/System/Vector2.hpp"
#include <array>
#include <set>


namespace Config
{
    inline constexpr int MaxBlocks = 5;
    inline constexpr int BoardSize = 12;
    inline constexpr int CellCount = BoardSize * BoardSize; 

    inline constexpr std::array<sf::Vector2i, 4> CardinalOffsets = 
    {
        sf::Vector2i{1, 0}, sf::Vector2i{0, 1}, 
        sf::Vector2i{-1, 0}, sf::Vector2i{0, -1}
    };

    inline constexpr std::array<sf::Vector2i, 4> DiagonalOffsets = {
        sf::Vector2i{1, 1}, sf::Vector2i{-1, 1},
        sf::Vector2i{1, -1}, sf::Vector2i{-1, -1}
    };

    inline constexpr int CanonicalCount = 21;
    inline constexpr int PolyominoCount = 91;
    inline constexpr int DeckSize = 18;
}

#endif