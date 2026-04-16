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
    initialzeKeys();
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
            mHeldPiecePtr->setPosition(worldPos);
        } 
    }

    if (auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left 
            && !mHeldPiecePtr) 
        {
            sf::Vector2i mousePos = mousePressed->position;
            sf::Vector2f worldPos = mWindow.mapPixelToCoords(mousePos);            
            if (auto piecePtr = mTrayPtr->getPieceAt(worldPos))
            {
                mHeldPiecePtr = piecePtr; 
                pushGrabCommand(worldPos, commands);
            }
        } 
    }

    if (auto keyPressed = event.getIf<sf::Event::KeyPressed>()) 
    { // Use function to set the id immediately to change shadow's id
        auto it = mKeyBinding.find(keyPressed->code);
        if (it != mKeyBinding.end() && mHeldPiecePtr) 
        {
            int pieceId = mHeldPiecePtr->getId(); 
            mHeldPiecePtr->setId(getTransformedId(pieceId, it->second));
        }
    }

    if (mHeldPiecePtr)
    { // Check shadow 60 times every second
        sf::Vector2i mousePos = sf::Mouse::getPosition(mWindow);
        sf::Vector2f worldPos = mWindow.mapPixelToCoords(mousePos);
        
        pushShadowCommand(worldPos, commands);
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

void Player::pushGrabCommand(sf::Vector2f worldPos, CommandQueue& commands) const
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

void Player::pushShadowCommand(sf::Vector2f worldPos, CommandQueue& commands) const
{
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

void Player::initialzeKeys()
{
    mKeyBinding[sf::Keyboard::Key::R] = RotateCW;
    mKeyBinding[sf::Keyboard::Key::C] = RotateCCW;
    mKeyBinding[sf::Keyboard::Key::H] = ReflectH;
    mKeyBinding[sf::Keyboard::Key::V] = ReflectV;
}

int Player::getTransformedId(int currentId, Transformation transform) const
{
    assert(currentId >= 0 && currentId < Blokus::PolyominoCount);
    const auto& library = Blokus::PolyominoGenerator::getData();

    switch(transform) 
    {
        case Transformation::RotateCW: 
        {
            return library.clockwiseRotatedIds.at(currentId);
        }

        case Transformation::RotateCCW: 
        {
            // Rotate CCW = Rotate CW three times
            int id = currentId;
            for (int i = 0; i < 3; ++i)
            {
                id = library.clockwiseRotatedIds.at(id);
            }
            
            return id;
        }

        case Transformation::ReflectH: 
        {
            return library.horizontallyReflectedIds.at(currentId);
        }

        case Transformation::ReflectV:
        {
            // Vertical Flip = Horizontal Flip -> Rotate CW -> Rotate CW
            int id = library.horizontallyReflectedIds.at(currentId);
            
            for (int i = 0; i < 2; ++i)
            {
                id = library.clockwiseRotatedIds.at(id);
            }

            return id; 
        }

        default:
        {
            assert(false && "Invalid Transformation!");
        }
    }
}


