#include "Nodes/PieceNode.hpp"
#include "Nodes/BlockNode.hpp"
#include "Config.hpp"

#include "SFML/Graphics/Rect.hpp"

#include <memory>
#include <iostream>


sf::IntRect getRect(Team team) {
    int tileSize = static_cast<int>(Config::TileSize);
    sf::Vector2i size = {tileSize, tileSize};

    switch(team) 
    {
        case Team::Blue:   
            return sf::IntRect({0, 0}, size);
        case Team::Yellow: 
            return sf::IntRect({tileSize, 0}, size);
        case Team::Red:    
            return sf::IntRect({0, tileSize}, size); 
        case Team::Green:  
            return sf::IntRect({tileSize, tileSize}, size);
        default:           
            return sf::IntRect({0, 0}, {0, 0});
    }
}

PieceNode::PieceNode(int pieceId, const Team team, TextureHolder& textures)
    : mCurrentId(pieceId)
    , mTeam(team)
    , mState(PieceState::Ready)
    , mTextures(textures)
{
   updateLayout();
}

int PieceNode::getId() const
{
    return mCurrentId;
}

void PieceNode::setId(int pieceId)
{
    mCurrentId = pieceId;

    updateLayout();
}

const Team PieceNode::getTeam() const
{
    return mTeam;
}

void PieceNode::updateLayout()
{
    clearChildren();

    const auto &library = Blokus::PolyominoGenerator::getData();
    const auto &coordinates = library.polyominoById.at(mCurrentId).blocks;

    for (const auto& pos : coordinates) 
    {
        auto block = std::make_unique<BlockNode>(
            mTextures.get(Textures::Tiles), getRect(mTeam));
        
        block->setPosition({pos.x * Config::GridSize, pos.y * Config::GridSize});
        attachChild(std::move(block));
    } 
}

void PieceNode::setState(PieceState state)
{
    mState = state;
}

PieceState PieceNode::getState() const
{
    return mState;
}

