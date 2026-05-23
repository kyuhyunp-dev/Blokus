#include "Player.hpp"

#include "Command/CommandQueue.hpp"
#include "Nodes/Arena.hpp"
#include "Nodes/PieceNode.hpp"
#include "Nodes/BoardNode.hpp"
#include "Query/TrayQuery.hpp"
#include "Query/BoardQuery.hpp"
#include "ClientUtility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <cassert>


Player::Player(sf::RenderWindow& window, const PolyominoDefinition& library, Referee& referee)
    : mWindow(window)
    , mReferee(referee)
    , mLibrary(library)
    , mTrayPtr(nullptr)
    , mBoardPtr(nullptr)
    , mHeldPiecePtr(nullptr)
    , mCurrentMousePos(0, 0)
{
    initializeKeys();
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
    if (auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            mCurrentMousePos = mousePressed->position;            
            sf::Vector2f worldPos = mWindow.mapPixelToCoords(mCurrentMousePos);   
            
            if (mHeldPiecePtr)
            {
                int pieceId = mHeldPiecePtr->getId();
                sf::Vector2i minSnappedGrid = mBoardPtr->getMinSnappedGrid(worldPos, mHeldPiecePtr->getCentroid());
                if (mReferee.isValid(pieceId, minSnappedGrid, mTeam))
                {
                    mReferee.place(pieceId, minSnappedGrid, mTeam);
                    pushPlaceCommand(minSnappedGrid, commands);
                }
                else 
                {
                    mHeldPiecePtr->setId(getCanonicalId(mHeldPiecePtr->getId()));
                    pushReturnCommand(commands); 
                }
                
                mHeldPiecePtr = nullptr;
                pushClearShadowCommand(commands);              
            }
            else if (auto piecePtr = mTrayPtr->getPieceAt(worldPos))
            {
                mHeldPiecePtr = piecePtr; 
                pushGrabCommand(worldPos, commands);
            }
        }
    }

    if (mHeldPiecePtr == nullptr) 
    {
        return;
    }

    if (auto keyPressed = event.getIf<sf::Event::KeyPressed>()) 
    { // Use function to set the id immediately to change shadow's id
        auto it = mKeyBinding.find(keyPressed->code);
        if (it != mKeyBinding.end()) 
        {
            int pieceId = mHeldPiecePtr->getId(); 
            mHeldPiecePtr->setId(getTransformedId(pieceId, it->second));
        }
    }

    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        mCurrentMousePos = mouseMoved->position;
        sf::Vector2f worldPos = mWindow.mapPixelToCoords(mCurrentMousePos);
        mHeldPiecePtr->setPosition(worldPos);
    } 
    
    sf::Vector2f worldPos = mWindow.mapPixelToCoords(mCurrentMousePos);
    pushShadowCommand(worldPos, commands);
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
    int pieceId = mHeldPiecePtr->getId();

    Command grab;
    grab.category = Category::Arena;
    grab.action = derivedAction<IArena>(
        [pieceId, worldPos](IArena& arena, sf::Time) 
    {
        arena.grabPiece(pieceId, worldPos);
    });

    commands.push(grab);
}

void Player::pushShadowCommand(sf::Vector2f worldPos, CommandQueue& commands) const
{
    assert(mHeldPiecePtr);
    sf::Vector2i minSnappedGrid = mBoardPtr->getMinSnappedGrid(worldPos, mHeldPiecePtr->getCentroid());
    
    int pieceId = mHeldPiecePtr->getId();
    sf::Color color = (mReferee.isValid(pieceId, minSnappedGrid, mTeam) ? 
        getShadowColor(mTeam) : getShadowColor(Team::None));

    Command shadow; 
    shadow.category = Category::Board;
    shadow.action = derivedAction<BoardNode>(
        [pieceId, minSnappedGrid, color](BoardNode& board, sf::Time) {
        board.updateShadow(pieceId, minSnappedGrid, color);
    });

    commands.push(shadow);    
}

void Player::pushPlaceCommand(sf::Vector2i minSnappedGrid, CommandQueue& commands) const 
{
    Command place;
    place.category = Category::Arena;
    place.action = derivedAction<IArena>(
        [minSnappedGrid](IArena& arena, sf::Time) 
    {
        arena.placePiece(minSnappedGrid);
    });

    commands.push(place);
}

void Player::pushReturnCommand(CommandQueue& commands) const
{
    Command returnCmd;
    returnCmd.category = Category::Arena;
    returnCmd.action = derivedAction<IArena>(
        [](IArena& arena, sf::Time) 
    {
        arena.returnPiece();
    });

    commands.push(returnCmd);
}

void Player::pushClearShadowCommand(CommandQueue& commands) const
{
    Command clearShadow;
    clearShadow.category = Category::Board;
    clearShadow.action = derivedAction<BoardNode>(
        [](BoardNode& board, sf::Time) 
        {
            board.clearShadow(); 
        });

    commands.push(clearShadow);
}

void Player::initializeKeys()
{
    mKeyBinding[sf::Keyboard::Key::R] = RotateCW;
    mKeyBinding[sf::Keyboard::Key::C] = RotateCCW;
    mKeyBinding[sf::Keyboard::Key::H] = ReflectH;
    mKeyBinding[sf::Keyboard::Key::V] = ReflectV;
}

int Player::getTransformedId(int currentId, Transformation transform) const
{
    assert(currentId >= 0 && currentId < Config::PolyominoCount);

    switch(transform) 
    {
        case Transformation::RotateCW: 
        {
            return mLibrary.clockwiseRotatedIds.at(currentId);
        }

        case Transformation::RotateCCW: 
        {
            // Rotate CCW = Rotate CW three times
            int id = currentId;
            for (int i = 0; i < 3; ++i)
            {
                id = mLibrary.clockwiseRotatedIds.at(id);
            }
            
            return id;
        }

        case Transformation::ReflectH: 
        {
            return mLibrary.horizontallyReflectedIds.at(currentId);
        }

        case Transformation::ReflectV:
        {
            // Vertical Flip = Horizontal Flip -> Rotate CW -> Rotate CW
            int id = mLibrary.horizontallyReflectedIds.at(currentId);
            
            for (int i = 0; i < 2; ++i)
            {
                id = mLibrary.clockwiseRotatedIds.at(id);
            }

            return id; 
        }

        default:
        {
            assert(false && "Invalid Transformation!");
            return -1;
        }
    }
}

int Player::getCanonicalId(int transformedId) const
{
    assert(transformedId >= 0 && transformedId < Config::PolyominoCount);

    return mLibrary.transformedToCanonical[transformedId];
}


