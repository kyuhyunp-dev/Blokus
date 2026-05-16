#ifndef MOCK_REFEREE_HPP
#define MOCK_REFEREE_HPP

#include <gmock/gmock.h>
#include "shared/Referee.hpp"

class MockReferee : public Referee 
{
public:
    // MOCK_METHOD(ReturnType, MethodName, (Args...), (Modifiers));
    MOCK_METHOD(void, place, (int pieceId, sf::Vector2i minOffset, Team team), (override));
    MOCK_METHOD(bool, isValid, (int pieceId, sf::Vector2i minOffset, Team team), (const, override));
};
#endif