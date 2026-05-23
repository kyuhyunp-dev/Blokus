#ifndef SHARED_UTILITY_HPP
#define SHARED_UTILITY_HPP

#include "SharedConfig.hpp"
#include "SFML/System/Vector2.hpp"

#include <set>


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
    return y * Config::BoardSize + x; 
}

inline sf::Vector2i getCoord(int index) 
{ 
    return { index % Config::BoardSize, index / Config::BoardSize }; 
}

#endif