#ifndef TRAYQUERY_HPP
#define TRAYQUERY_HPP

#include <SFML/System/Vector2.hpp>
#include <optional>

class TrayQuery
{
public:
    virtual ~TrayQuery() = default;
    virtual PieceNode* getPieceAt(sf::Vector2f worldPos) const = 0;
};

#endif