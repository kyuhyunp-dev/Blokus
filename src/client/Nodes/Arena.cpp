#include "Nodes/Arena.hpp"
#include "Nodes/BoardNode.hpp"
#include "Nodes/TrayNode.hpp"
#include "shared/Team.hpp"
#include "Command/CommandQueue.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Mouse.hpp>

#include <memory>


Arena::Arena(sf::RenderTarget& target, TextureHolder& textures, 
    const PolyominoDefinition& library,
    std::array<int, Config::DeckSize> deck, 
    CommandQueue& commands, Team team)
    : mTarget(target)
    , mTextures(textures)
    , mLibrary(library)
    , mSceneLayers()
    , mBoardPtr(nullptr)
    , mTrayPtr(nullptr)
    , mCommands(commands)
    , mActiveTeam(Team::Red)
    , mTeam(team)
    , mDeck(deck)
{
}

void Arena::buildScene()
{
    // Create layer containers (SceneNode instances to hold children by layer)
    for (int i = 0; i < LayerCount; ++i)
    {
        auto layer = std::make_unique<SceneNode>();
        mSceneLayers[i] = layer.get();

        SceneNode::attachChild(std::move(layer));
    }

    auto board = std::make_unique<BoardNode>(mLibrary);
    mBoardPtr = board.get();
    board->setPosition({ Config::Padding, Config::Padding });  
    mSceneLayers[World]->attachChild(std::move(board));

    auto boardBounds = mBoardPtr->getGlobalBounds();

    auto tray = std::make_unique<TrayNode>(mTextures);
    mTrayPtr = tray.get();
    mTrayPtr->setPosition({ 
        boardBounds.position.x + boardBounds.size.x + Config::Padding, // 10px gap
        boardBounds.position.y 
    });    
    mSceneLayers[World]->attachChild(std::move(tray));

    // Place pieces in the World componenet 
    for (int slotId = 0; slotId < Config::DeckSize; ++slotId) {
        int pieceId = mDeck[slotId];
        auto piece = std::make_unique<PieceNode>(pieceId, mTeam, mTextures, mLibrary);
        mTrayPtr->addPiece(slotId, std::move(piece));
    }
}

unsigned int Arena::getCategory() const
{
    return Category::Arena;
}

void Arena::grabPiece(int id, sf::Vector2f worldPos) 
{
    assert(id >= 0 && id < Config::PolyominoCount && "Invalid ID");
    //  Pull from Tray (The Tray identifies it by ID now, not position)
    auto piece = mTrayPtr->withdrawPiece(id);
    
    // Transformation & Handoff
    piece->setOrigin(piece->getCentroid());
    piece->setPosition(worldPos); 
    
    mActivePiecePtr = piece.get();
    mSceneLayers[Action]->attachChild(std::move(piece));
}

void Arena::placePiece(sf::Vector2i gridCoord)
{
    assert(mActivePiecePtr);
    auto node = mSceneLayers[Action]->detachChild(*mActivePiecePtr); 
    SceneNode* nodePtr = node.release();

    assert(dynamic_cast<PieceNode*>(nodePtr));
    std::unique_ptr<PieceNode> piece(static_cast<PieceNode*>(nodePtr));

    piece->setOrigin({0, 0});
    mBoardPtr->addPiece(std::move(piece), gridCoord);

    mActivePiecePtr = nullptr;
}

void Arena::returnPiece()
{
    assert(mActivePiecePtr);
    auto node = mSceneLayers[Action]->detachChild(*mActivePiecePtr); 
    SceneNode* nodePtr = node.release();
    
    assert(dynamic_cast<SceneNode*>(nodePtr));
    std::unique_ptr<PieceNode> piece(static_cast<PieceNode*>(nodePtr));

    auto slotId = piece->getSlotId();
    assert(slotId.has_value() && "Piece does not have a slot ID to return to!");

    piece->setOrigin({0.f, 0.f});
    mTrayPtr->addPiece(slotId.value(), std::move(piece));
    
    mActivePiecePtr = nullptr;
}

TrayNode* Arena::getTrayNodePtr() const
{
    return mTrayPtr;
}

BoardNode* Arena::getBoardNodePtr() const
{
    return mBoardPtr;
}

SceneNode* Arena::getLayer(Layer layer) const 
{ 
    return mSceneLayers[layer]; 
}


void Arena::loadTextures()
{
    std::string tileFile = "client/textures/tiles.png";
    mTextures.load(Textures::ID::Tiles, tileFile); 
}
