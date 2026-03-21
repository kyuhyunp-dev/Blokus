#include "Arena.hpp"
#include "Nodes/BoardNode.hpp"
#include "Nodes/TrayNode.hpp"
#include "Nodes/InteractionNode.hpp"
#include "shared/Team.hpp"
#include "Path.hpp"

#include <memory>


Arena::Arena(std::array<int, Blokus::DeckSize> deck)
    : mTextures()
    , mSceneLayers()
    , mSceneGraph()
    , mBoard(nullptr)
    , mTray(nullptr)
    , mActiveTeam(Team::Red)
    , mTeam(Team::Red)
    , mDeck(deck)
{
    loadTextures();
    buildScene();
}

// The "Engine" - Processes the commands sent by the Player
void Arena::update(sf::Time dt)
{
    // CommandQueue processing will be implemented in next PR
    // For now, this just updates the scene hierarchy
    mSceneGraph.update(dt);
}

void Arena::loadTextures()
{
    std::string tileFile = getAssetPath("client/textures/tiles.png");
    mTextures.load(Textures::ID::Tiles, tileFile);
}

void Arena::buildScene()
{
    // Create layer containers (SceneNode instances to hold children by layer)
    for (int i = 0; i < LayerCount; ++i)
    {
        auto layer = std::make_unique<SceneNode>();
        mSceneLayers[i] = layer.get();

        mSceneGraph.attachChild(std::move(layer));
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

void Arena::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mSceneGraph, states);
}
