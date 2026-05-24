#ifndef STATESTACK_HPP
#define STATESTACK_HPP

#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "Resource/ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>

#include <vector>
#include <utility>
#include <functional>
#include <map>
#include <cassert>


namespace sf
{
	class Event;
	class RenderWindow;
}

class StateStack
{
	friend class StateStackTest;

public:
	enum Action
	{
		Push,
		Pop,
		Clear,
	};

	explicit StateStack(State::Context context);

	// Non copyable
	StateStack(const StateStack&) = delete;
	StateStack& operator=(const StateStack&) = delete;

	template <typename T>
	void registerState(States::ID stateID);

	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);

	void pushState(States::ID stateID);
	void popState();
	void clearStates();

	bool isEmpty() const;

protected:
	struct PendingChange
	{
		explicit PendingChange(Action action, States::ID stateID = States::ID::None);

		Action action;
		States::ID stateID;
	};
	
    const std::vector<PendingChange>& getPendingChanges() const { 
        return mPendingList; 
    }

private:
	State::Ptr createState(States::ID stateID);
	void applyPendingChanges();

	std::vector<State::Ptr> mStack;
	std::vector<PendingChange> mPendingList;

	State::Context mContext;
	std::map<States::ID, std::function<State::Ptr()>> mFactories;
};

template <typename T>
void StateStack::registerState(States::ID stateID)
{
	mFactories[stateID] = [this]()
		{
			return State::Ptr(new T(*this, mContext));
		};
}

#endif