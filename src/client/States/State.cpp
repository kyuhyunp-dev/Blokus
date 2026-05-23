#include "States/State.hpp"
#include "States/StateStack.hpp"


State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, 
	FontHolder& fonts, GameSessionData& gameSessionData, NetworkClient& networkClient,
	const PolyominoDefinition& library)
	: window(&window)
	, textures(&textures)
	, fonts(&fonts)
	, gameSessionData(&gameSessionData)
	, networkClient(&networkClient)
	, library(&library)
{
}

State::State(StateStack& stack, Context context)
	: mStack(&stack)
	, mContext(context)
{
}

State::~State()
{
}

void State::requestStackPush(States::ID stateID)
{
	mStack->pushState(stateID);
}

void State::requestStackPop()
{
	mStack->popState();
}

void State::requestStateClear()
{
	mStack->clearStates();
}

State::Context State::getContext() const
{
	return mContext;
}

void State::onActivate()
{
}

void State::onDestroy()
{
}