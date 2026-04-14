#ifndef BOARDQUERY_HPP
#define BOARDQUERY_HPP

#include <SFML/System/Vector2.hpp>

class BoardQuery {
public:
    virtual ~BoardQuery() = default;
    virtual sf::Vector2i getMinSnappedGrid(sf::Vector2f worldPos, sf::Vector2f minOffset) const = 0;
};

#endif