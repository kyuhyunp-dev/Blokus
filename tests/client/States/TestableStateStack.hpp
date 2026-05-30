#ifndef TESTABLE_STATE_STACK_HPP
#define TESTABLE_STATE_STACK_HPP

#include "States/StateStack.hpp"


class TestableStateStack : public StateStack 
{
public:
    using StateStack::StateStack; 

    using StateStack::getPendingChanges; 
};

#endif