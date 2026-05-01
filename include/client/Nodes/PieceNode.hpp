#ifndef PIECENODE_HPP
#define PIECENODE_HPP

#include "Nodes/SceneNode.hpp"
#include "shared/Team.hpp"
#include "shared/PolyominoUtil.hpp"
#include "shared/Constants.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Nodes/SceneNode.hpp"
#include "Command/Category.hpp"
#include "Utility.hpp"

#include <optional>


enum class PieceState
{
    Ready,
    Moving,
    Placed
};

class PieceNode : public SceneNode 
{
public:
    explicit PieceNode(int pieceId, const Team team, TextureHolder& textures);

    int getId() const;

    void setId(int pieceId);

    const Team getTeam() const;

    sf::Vector2f getCentroid() const;

    void setSlotId(int slotId);

    std::optional<int> getSlotId() const;

private:
    void updateLayout();

    void setCentroid();

private:
    int mCurrentId;
    std::optional<int> mSlotId;

    const Team mTeam;
    Category::Type mCategory;
    TextureHolder& mTextures;

    sf::Vector2f mCentroid;
};

#endif