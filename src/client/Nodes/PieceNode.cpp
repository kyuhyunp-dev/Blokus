#include "Nodes/PieceNode.hpp"
#include "Nodes/BlockNode.hpp"

#include <memory>


PieceNode::PieceNode(int pieceId, const Team team, 
    TextureHolder& textures, const PolyominoDefinition& library)
    : mCurrentId(pieceId)
    , mSlotId(std::nullopt)
    , mTeam(team)
    , mTextures(textures)
    , mLibrary(library)
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

    const auto &coordinates = mLibrary.polyominoById.at(mCurrentId).blocks;

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
    const auto &coordinates = mLibrary.polyominoById.at(mCurrentId).blocks;

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