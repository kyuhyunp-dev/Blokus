#ifndef TRAYNODE_HPP
#define TRAYNODE_HPP

#include "SceneNode.hpp"
#include "PieceNode.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Config.hpp"
#include "Query/TrayQuery.hpp"

#include <map>
#include <vector>
#include <memory>


class TrayNode : public SceneNode, public TrayQuery
{
public:
	using PiecePtr = std::unique_ptr<PieceNode>;    

    public:
    TrayNode(TextureHolder& textures);

    // Initial setup: Adds a piece to a specific logical slot
    void addPiece(int slotId, PiecePtr piece);

    // The "Dumb" extraction: Just gives up ownership of the piece at slotId
    // The Player class decides when to call this based on getSlotManifest()
    PiecePtr withdrawPiece(int pieceId);

    PieceNode* getPieceAt(sf::Vector2f worldPos) const override;

private:
    // SceneNode overrides
    virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
    
    // Internal helper to calculate the top-left coordinate of a slot ID
    sf::Vector2f calculateSlotPosition(int slotId) const;

private:
    TextureHolder& mTextures;

    // Storage for pieces indexed by their 0-17 Slot ID
    std::map<int, PieceNode*> mSlots;

    // Layout Constants (3x5 blocks per slot)
    const int   mColumns     = 6;
    const float mSlotWidth   = 3.f * Config::GridSize;
    const float mSlotHeight  = 5.f * Config::GridSize;
    const float mPadding     = 10.f; // Space between 3x5 slots
};

#endif