#ifndef BLOKUS_CONSTANTS_HPP
#define BLOKUS_CONSTANTS_HPP

#include "SFML/System/Vector2.hpp"
#include <array>
#include <set>


namespace Blokus
{
    inline constexpr int kMaxBlocks = 5;
    inline constexpr int kBoardSize = 10;

    inline constexpr std::array<sf::Vector2i, 4> kCardinalOffsets = 
    {
        sf::Vector2i{1, 0}, sf::Vector2i{0, 1}, 
        sf::Vector2i{-1, 0}, sf::Vector2i{0, -1}
    };


    struct Vector2iCompare 
    {
        bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const 
        {
            if (a.x != b.x) 
            { // Sort by X first
                return a.x < b.x; 
            }

            // Then by Y
            return a.y < b.y; 
        }
    };

    using Polyomino = std::set<sf::Vector2i, Vector2iCompare>;
}

#endif