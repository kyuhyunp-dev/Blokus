#include "Player.hpp"

#include "Config.hpp"
#include "Command/CommandQueue.hpp"
#include "Referee.hpp"
#include "Nodes/Arena.hpp"
#include "Nodes/PieceNode.hpp"
#include "Nodes/BoardNode.hpp"
#include "Query/TrayQuery.hpp"
#include "Query/BoardQuery.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>


Player::Player(sf::RenderWindow& window)
    : mWindow(window)
    , mTrayPtr(nullptr)
    , mBoardPtr(nullptr)
    , mHeldPiecePtr(nullptr) 
{
}

void Player::setQuery(TrayQuery* trayPtr, BoardQuery* boardPtr)
{
    assert(trayPtr != nullptr);
    mTrayPtr = trayPtr;

    assert(boardPtr != nullptr);
    mBoardPtr = boardPtr;
}

void Player::setTeam(Team team)
{
    mTeam = team;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if (mHeldPiecePtr)
        {
            sf::Vector2i mousePos = mouseMoved->position;
            sf::Vector2f worldPos = mWindow.mapPixelToCoords(mousePos);
            pushMoveCommand(worldPos, commands);
        }
    }

    if (auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        // If mouse touched a piece
        if (mousePressed->button == sf::Mouse::Button::Left 
            && !mHeldPiecePtr) //mReferee.canGrab ->optional PiecePtr
        {
            // 1. Ask Referee: "Is there a piece at this mouse coordinate?"
            // Referee returns the Piece ID if found, otherwise nullopt.
            sf::Vector2i mousePos = mousePressed->position;
            sf::Vector2f worldPos = mWindow.mapPixelToCoords(mousePos);            
            if (auto piecePtr = mTrayPtr->getPieceAt(worldPos))
            {
                mHeldPiecePtr = piecePtr; 
                pushGrabCommand(worldPos, commands);
            }
        } 
    }
}

std::optional<int> Player::getHeldPieceId() const
{
    if (mHeldPiecePtr) {
        return mHeldPiecePtr->getId();
    }

    return std::nullopt;
}

Team Player::getTeam() const
{
    return mTeam;
}

void Player::pushGrabCommand(sf::Vector2f worldPos, CommandQueue& commands)
{
    Command grab;
    grab.category = Category::Arena;
    grab.action = derivedAction<Arena>(
        [this, worldPos](Arena& arena, sf::Time) 
    {
        arena.grabPiece(mHeldPiecePtr->getId(), worldPos);
    });

    commands.push(grab);
}

void Player::pushMoveCommand(sf::Vector2f worldPos, CommandQueue& commands)
{
    Command move;
    move.category = Category::ActivePiece;
    move.action = derivedAction<PieceNode>(
        [worldPos](PieceNode& piece, sf::Time) 
    {
        piece.setPosition(worldPos);
    });

    commands.push(move); 

    assert(mHeldPiecePtr);
    sf::Vector2i minSnappedGrid = mBoardPtr->getMinSnappedGrid(worldPos, mHeldPiecePtr->getCentroid());
    
    int pieceId = mHeldPiecePtr->getId();
    sf::Color color = Config::getShadowColor(mTeam);

    Command shadow;
    shadow.category = Category::Board;
    shadow.action = derivedAction<BoardNode>([pieceId, minSnappedGrid, color](BoardNode& board, sf::Time) {
        board.updateShadow(pieceId, minSnappedGrid, color);
    });
    commands.push(shadow);    
}


