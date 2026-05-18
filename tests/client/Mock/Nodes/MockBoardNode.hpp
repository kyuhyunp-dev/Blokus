#ifndef MOCK_BOARD_NODE_HPP
#define MOCK_BOARD_NODE_HPP

#include <gmock/gmock.h>
#include "Nodes/BoardNode.hpp"

class MockBoardNode : public BoardNode 
{
public:
    MockBoardNode() : BoardNode() {}

    MOCK_METHOD(void, updateShadow, (int pieceId, sf::Vector2i minSnappedGrid, sf::Color color), (override));
    MOCK_METHOD(void, clearShadow, (), (override));
};

#endif