#ifndef BLOKUS_CONSTANTS_HPP
#define BLOKUS_CONSTANTS_HPP

#include "SFML/System/Vector2.hpp"
#include <array>
#include <set>


namespace Blokus
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
        bool operator()(const Blocks& lhs, const Blocks& rhs) const 
        {
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

    const int CanonicalCount = 21;
    const int PolyominoCount= 91;
    
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
        }
    };

    inline int getIndex(int x, int y) 
    { 
        return y * BoardSize + x; 
    }

    inline sf::Vector2i getCoord(int index) 
    { 
        return { index % BoardSize, index / BoardSize }; 
    }

    const int DeckSize = 18;
}


#endif