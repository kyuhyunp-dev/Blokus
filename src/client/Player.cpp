#include "Player.hpp"

#include "Command/CommandQueue.hpp"
#include "Referee.hpp"
#include "Nodes/Arena.hpp"
#include "Nodes/PieceNode.hpp"
#include "Query/TrayQuery.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


Player::Player(sf::RenderWindow& window)
    : mWindow(window)
    , mTray(nullptr)
    , mHeldPieceId(std::nullopt) 
{
}

void Player::setQuery(TrayQuery* tray)
{
    assert(tray != nullptr);
    mTray = tray;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if (mHeldPieceId)
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
            && !mHeldPieceId) //mReferee.canGrab ->optional PiecePtr
        {
            // 1. Ask Referee: "Is there a piece at this mouse coordinate?"
            // Referee returns the Piece ID if found, otherwise nullopt.
            sf::Vector2i mousePos = mousePressed->position;
            sf::Vector2f worldPos = mWindow.mapPixelToCoords(mousePos);            
            if (auto id = mTray->getPieceIdAt(worldPos))
            {
                pushGrabCommand(id.value(), worldPos, commands);
                mHeldPieceId = id; 
            }
        } 
    }
}

std::optional<int> Player::getHeldPieceId() const
{
    return mHeldPieceId;
}


void Player::pushGrabCommand(int id, sf::Vector2f worldPos, CommandQueue& commands)
{
    Command grab;
    grab.category = Category::Arena;
    grab.action = derivedAction<Arena>(
        [id, worldPos](Arena& arena, sf::Time) 
    {
        arena.grabPiece(id, worldPos);
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
}
