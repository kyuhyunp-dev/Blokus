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
    , mSlotId(std::nullopt)
    , mTeam(team)
    , mTextures(textures)
    , mCentroid()
{
   updateLayout();
   setCentroid();
}

int PieceNode::getId() const
{
    return mCurrentId;
}

void PieceNode::setId(int pieceId)
{
    mCurrentId = pieceId;

    updateLayout();
    setCentroid();
    setOrigin(mCentroid);
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

void PieceNode::setCentroid()
{
    const auto &library = Blokus::PolyominoGenerator::getData();
    const auto &coordinates = library.polyominoById.at(mCurrentId).blocks;

    mCentroid = { 0.f, 0.f };
    for (const auto& pos : coordinates) 
    {
        mCentroid.x += pos.x * Config::GridSize + Config::GridSize / 2.f;
        mCentroid.y += pos.y * Config::GridSize + Config::GridSize / 2.f;
    } 

    mCentroid = { 
        mCentroid.x / coordinates.size() , 
        mCentroid.y / coordinates.size() 
    };
}

sf::Vector2f PieceNode::getCentroid() const
{
    return mCentroid;
}

void PieceNode::setSlotId(int slotId) 
{
    mSlotId = slotId;
}

std::optional<int> PieceNode::getSlotId() const
{
    return mSlotId;
}

void PieceNode::setCategoryType(Category::Type category)
{
    mCategory = category;
}

unsigned int PieceNode::getCategory() const
{
    return mCategory;
}
