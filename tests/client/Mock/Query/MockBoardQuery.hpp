#include "Query/BoardQuery.hpp"

// A simple mock for the TrayQuery interface
class MockBoardQuery : public BoardQuery {
public:
    sf::Vector2i getMinSnappedGrid(sf::Vector2f worldPos, sf::Vector2f minOffset) const override
    {
        return {100, 100};
    }

};