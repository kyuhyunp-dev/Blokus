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

    inline constexpr std::array<sf::Vector2i, 4> kDiagonalOffsets = {
        sf::Vector2i{1, 1}, sf::Vector2i{-1, 1},
        sf::Vector2i{1, -1}, sf::Vector2i{-1, -1}
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

    using Blocks = std::set<sf::Vector2i, Vector2iCompare>;

    struct PolyominoComparator {
        bool operator()(const Blocks& lhs, const Blocks& rhs) const {
            // Example: Sort by size first
            if (lhs.size() != rhs.size()) {
                return lhs.size() < rhs.size();
            }
            
            // If sizes are equal, use the default lexicographical comparison
            return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                Vector2iCompare() 
            );
        }
    };

    const static int kCanonicalCount = 21;
    const static int kPolyominoCount = 91;
    
    struct Polyomino
    {
        Blocks blocks;

        struct 
        {
            Blocks edges;
            Blocks corners;

        } sensors;

        Polyomino(Blocks piece)
            : blocks(std::move(piece))
        {}

        Polyomino()
        {
           blocks.insert({0, 0});
        }
    };
    
}

#endif