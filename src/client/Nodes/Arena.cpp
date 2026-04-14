#include "Nodes/Arena.hpp"
#include "Nodes/BoardNode.hpp"
#include "Nodes/TrayNode.hpp"
#include "shared/Team.hpp"
#include "Path.hpp"
#include "Command/CommandQueue.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Mouse.hpp>

#include <memory>


Arena::Arena(sf::RenderTarget& target, TextureHolder& textures, std::array<int, Blokus::DeckSize> deck, CommandQueue& commands, Team team)
    : mTarget(target)
    , mTextures(textures)
    , mSceneLayers()
    , mBoard(nullptr)
    , mTray(nullptr)
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

    auto board = std::make_unique<BoardNode>(mTextures);
    mBoard = board.get();
    board->setPosition({ Config::Padding, Config::Padding });  
    mSceneLayers[World]->attachChild(std::move(board));

    auto boardBounds = mBoard->getGlobalBounds();

    auto tray = std::make_unique<TrayNode>(mTextures);
    mTray = tray.get();
    mTray->setPosition({ 
        boardBounds.position.x + boardBounds.size.x + Config::Padding, // 10px gap
        boardBounds.position.y 
    });    
    mSceneLayers[World]->attachChild(std::move(tray));

    // Place pieces in the World componenet 
    for (int slotId = 0; slotId < Blokus::DeckSize; ++slotId) {
        int pieceId = mDeck[slotId];
        auto piece = std::make_unique<PieceNode>(pieceId, mTeam, mTextures);
        mTray->addPiece(slotId, std::move(piece));
    }
}

unsigned int Arena::getCategory() const
{
    return Category::Arena;
}

void Arena::grabPiece(int id, sf::Vector2f worldPos) 
{
    assert(id >= 0 && id < Blokus::PolyominoCount && "Invalid ID");
    //  Pull from Tray (The Tray identifies it by ID now, not position)
    auto piece = mTray->withdrawPiece(id);
    
    // Transformation & Handoff
    piece->setOrigin(piece->getCentroid());
    piece->setPosition(worldPos); 
    
    piece->setCategoryType(Category::ActivePiece);
    mSceneLayers[Action]->attachChild(std::move(piece));
}

TrayNode* Arena::getTrayNode() const
{
    return mTray;
}

BoardNode* Arena::getBoardNode() const
{
    return mBoard;
}

SceneNode* Arena::getLayer(Layer layer) const 
{ 
    return mSceneLayers[layer]; 
}


