#ifndef MOCK_BOARD_QUERY_HPP
#define MOCK_BOARD_QUERY_HPP

#include "Query/BoardQuery.hpp"


// A simple mock for the TrayQuery interface
class MockBoardQuery : public BoardQuery 
{
public:
    void setMinSnappedGrid(sf::Vector2i grid) 
    {
        mReturnValue = grid;
    }

    sf::Vector2i getMinSnappedGrid(sf::Vector2f worldPos, sf::Vector2f minOffset) const override
    {
        return mReturnValue;
    }

private:
    sf::Vector2i mReturnValue = {0, 0};
};

#endif