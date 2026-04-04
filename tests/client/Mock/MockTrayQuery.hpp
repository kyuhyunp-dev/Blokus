#include "Query/TrayQuery.hpp"

// A simple mock for the TrayQuery interface
class MockTrayQuery : public TrayQuery {
public:
    // We tell the mock exactly what to return for testing
    std::optional<int> responseId = std::nullopt;

    std::optional<int> getPieceIdAt(sf::Vector2f worldPos) const override {
        return responseId;
    }
};