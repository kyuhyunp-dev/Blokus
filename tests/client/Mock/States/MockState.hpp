#ifndef MOCK_STATE_HPP
#define MOCK_STATE_HPP

#include <gmock/gmock.h>
#include "States/State.hpp"

// 1. Create a Mock State to track method calls
class MockState : public State {
public:
    // Change access to public for tests
    using State::requestStackPush;
    using State::requestStackPop;
    using State::requestStateClear;

    MockState(StateStack& stack, Context context) 
    : State(stack, context) {}

    MOCK_METHOD(void, draw, (), (override));
    MOCK_METHOD(bool, update, (sf::Time), (override));
    MOCK_METHOD(bool, handleEvent, (const sf::Event&), (override));
    MOCK_METHOD(void, onActivate, (), (override));
    MOCK_METHOD(void, onDestroy, (), (override));
};

#endif 