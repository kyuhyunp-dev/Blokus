#ifndef MOCK_BOARD_NODE_HPP
#define MOCK_BOARD_NODE_HPP

#include <gmock/gmock.h>
#include "Nodes/BoardNode.hpp"
#include "shared/PolyominoUtil.hpp"


class MockBoardNode : public BoardNode 
{
public:
    MockBoardNode(const PolyominoDefinition& library) 
    : BoardNode(library) 
    {} 

    MOCK_METHOD(void, updateShadow, (int pieceId, sf::Vector2i minSnappedGrid, sf::Color color), (override));
    MOCK_METHOD(void, clearShadow, (), (override));
};

#endif