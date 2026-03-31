#ifndef PIECENODE_HPP
#define PIECENODE_HPP

#include "Nodes/SceneNode.hpp"
#include "shared/Team.hpp"
#include "shared/PolyominoUtil.hpp"
#include "shared/Constants.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Nodes/SceneNode.hpp"


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

    void setState(PieceState state);

    PieceState getState() const;

    sf::Vector2f getCentroid() const;

private:
    void updateLayout();

    void setCentroid();

private:
    int mCurrentId;
    const Team mTeam;
    PieceState mState;
    TextureHolder& mTextures;

    sf::Vector2f mCentroid;
};

#endif