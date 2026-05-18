#ifndef MOCK_TRAY_QUERY_HPP
#define MOCK_TRAY_QUERY_HPP

#include <gmock/gmock.h>
#include "Query/TrayQuery.hpp"
#include "Nodes/PieceNode.hpp"

class MockTrayQuery : public TrayQuery {
public:
    MOCK_METHOD(PieceNode*, getPieceAt, (sf::Vector2f worldPos), (const, override));
};

#endif