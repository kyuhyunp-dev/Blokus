#ifndef MOCK_BOARD_NODE_HPP
#define MOCK_BOARD_NODE_HPP

#include "Nodes/BoardNode.hpp"
#include "Mock/Resource/MockTextureHolder.hpp"


class MockBoardNode : public BoardNode 
{
public:
    MockBoardNode(MockTextureHolder& textures)
        : BoardNode(textures) 
    {}

    void updateShadow(int pieceId, sf::Vector2i coord, sf::Color color) override 
    {
        lastPieceId = pieceId;
        lastCoord = coord;
        lastColor = color;
    }

public:
    int lastPieceId;
    sf::Vector2i lastCoord;
    sf::Color lastColor;
}; 

#endif