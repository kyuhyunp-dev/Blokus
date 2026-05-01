#ifndef MOCK_ARENA_HPP
#define MOCK_ARENA_HPP

#include "Nodes/Arena.hpp"

class MockArena : public Arena 
{
public:
    // Inherit the constructor from Arena
    using Arena::Arena; 

    // Spy variables to track what the Command does
    bool placePieceCalled = false;
    sf::Vector2i lastPlacedGrid = {-1, -1};

    // Override the function to intercept the call
    void placePiece(sf::Vector2i gridCoord) override 
    {
        placePieceCalled = true;
        lastPlacedGrid = gridCoord;
    }
};

#endif