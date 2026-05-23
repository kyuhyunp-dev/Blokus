#include "States/NetworkGameState.hpp"
#include "Nodes/Arena.hpp"
#include "Path.hpp"
#include "shared/Team.hpp"
#include "ClientConfig.hpp"
#include "Nodes/TrayNode.hpp"
#include "Nodes/BoardNode.hpp"
#include "States/State.hpp"

#include <stdexcept>


NetworkGameState::NetworkGameState(StateStack& stack, Context context)
    : State(stack, context)
    , mCommandQueue()
    , mReferee(*context.library)
    , mArenaPtr(nullptr) 
    , mPlayers()
    , mLocalPlayerIdentifier()
{    
}

void NetworkGameState::onActivate()
{
    // TODO: Move to handleUpdate's SpawnSelf 
    mArenaPtr = createArena();
    mArenaPtr->loadTextures();
    mArenaPtr->buildScene();

    mLocalPlayerIdentifier = 0;
    mPlayers[mLocalPlayerIdentifier].reset(new Player(*getContext().window, *getContext().library, mReferee));
    mPlayers[mLocalPlayerIdentifier]->setTeam(Team::Red);

    setQuery(); 
}

void NetworkGameState::draw()
{
    getContext().window->draw(*mArenaPtr);
}

bool NetworkGameState::update(sf::Time dt)
{
    while (!mCommandQueue.isEmpty())
    {
        // Arena receives the command and tells all its children (Tray, Board, ActiveNode)
        mArenaPtr->onCommand(mCommandQueue.pop(), dt);
    }

    mArenaPtr->update(dt);
    return true;
}

bool NetworkGameState::handleEvent(const sf::Event& event)
{
	for (auto& pair : mPlayers)
	{ 
		pair.second->handleEvent(event, mCommandQueue);
	}

    return true;
}

std::unique_ptr<IArena> NetworkGameState::createArena()
{
    // Precondition: must be unique
    const std::array<int, Config::DeckSize> deck = 
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}; 

    // Production code returns the real Arena
    return std::make_unique<Arena>(
        *getContext().window, 
        *getContext().textures,
        *getContext().library, 
        deck, 
        mCommandQueue, 
        Team::Red 
    );   
}

void NetworkGameState::setQuery()
{
    assert(dynamic_cast<TrayQuery*>(mArenaPtr->getTrayNodePtr()) != nullptr);
    TrayQuery* trayPtr = static_cast<TrayQuery*>(mArenaPtr->getTrayNodePtr());

    assert(dynamic_cast<BoardQuery*>(mArenaPtr->getBoardNodePtr()) != nullptr);
    BoardQuery* boardPtr = static_cast<BoardQuery*>(mArenaPtr->getBoardNodePtr());

    mPlayers[mLocalPlayerIdentifier]->setQuery(trayPtr, boardPtr);
}

CommandQueue& NetworkGameState::getCommandQueue()
{
    return mCommandQueue;
}
