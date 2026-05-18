#ifndef MOCK_BOARD_QUERY_HPP
#define MOCK_BOARD_QUERY_HPP

#include <gmock/gmock.h>
#include "Query/BoardQuery.hpp"

class MockBoardQuery : public BoardQuery 
{
public:
    MOCK_METHOD(sf::Vector2i, getMinSnappedGrid, (sf::Vector2f worldPos, sf::Vector2f minOffset), (const, override));
};

#endif