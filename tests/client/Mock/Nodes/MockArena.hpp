#ifndef MOCK_ARENA_HPP
#define MOCK_ARENA_HPP

#include "Nodes/Arena.hpp"
#include <memory>


class MockArena : public Arena 
{
public:
   MockArena(sf::RenderTarget& target, MockTextureHolder& textures, 
              CommandQueue& commands, Team team)
        : Arena(target, textures, 
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}, 
            commands, team) 
    {
    }

    // Spy variables to track what the Command does
    bool placePieceCalled = false;
    bool returnPieceCalled = false;
    sf::Vector2i lastPlacedGrid = {-1, -1};

    void placePiece(sf::Vector2i gridCoord) override 
    {
        placePieceCalled = true;
        lastPlacedGrid = gridCoord;
    }

    void returnPiece() override 
    {
        returnPieceCalled = true;
    }
};

#endif