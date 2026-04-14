#include "Nodes/TrayNode.hpp" 

#include <SFML/Graphics/RectangleShape.hpp>


TrayNode::TrayNode(TextureHolder& textures)
    : mTextures(textures)
{
}

// Initial setup: Adds a piece to a specific logical slot
void TrayNode::addPiece(int slotId, PiecePtr piece)
{ 
    assert(slotId >= 0 && slotId < Blokus::DeckSize);
    
    // Position the piece within the Tray's local space
    piece->setPosition(calculateSlotPosition(slotId));

    // Store a raw pointer for quick lookup (The "Ref")
    // This allows withdrawPiece(slotId) to know WHICH child to detach.
    mSlots[slotId] = piece.get();  

    // Transfer ownership to the SceneNode hierarchy
    // This handles the 'private draw' issue automatically.
    this->attachChild(std::move(piece));
}

// The "Dumb" extraction: Just gives up ownership of the piece at slotId
// The Player class decides when to call this based on getSlotManifest()
TrayNode::PiecePtr TrayNode::withdrawPiece(int pieceId)
{
    // Check if the slot actually has a piece (isOccupied logic)
    auto it = std::find_if(mSlots.begin(), mSlots.end(), 
    [pieceId](const auto& pair) {
        return pair.second->getId() == pieceId;
    });
    assert(it != mSlots.end() && "Piece ID not found in Tray slots!"); 

    // Remove the entry from the map (the slot is now "not occupied")
    PieceNode* target = it->second;
    target->setSlotId(it->first);

    mSlots.erase(it);
    auto detached = detachChild(*target);
    assert(dynamic_cast<PieceNode*>(detached.get()) != nullptr);
    return PiecePtr(static_cast<PieceNode*>(detached.release()));
}

void TrayNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RectangleShape slotShape({mSlotWidth, mSlotHeight});
    slotShape.setFillColor(sf::Color(255, 255, 255, 20)); // Faint white
    slotShape.setOutlineColor(sf::Color(255, 255, 255, 50));
    slotShape.setOutlineThickness(1.f);

    for (int i = 0; i < Blokus::DeckSize; ++i) {
        slotShape.setPosition(calculateSlotPosition(i));
        target.draw(slotShape, states);
    }
}
    
// Internal helper to calculate the top-left coordinate of a slot ID
sf::Vector2f TrayNode::calculateSlotPosition(int slotId) const
{
    int col = slotId % mColumns;
    int row = slotId / mColumns;

    float x = col * (mSlotWidth + mPadding);
    float y = row * (mSlotHeight + mPadding);

    return { x, y };
}

PieceNode* TrayNode::getPieceAt(sf::Vector2f worldPos) const 
{
    for (auto const& [slotId, piece] : mSlots) {
        if (piece->contains(worldPos)) {
            return piece; // We found a piece!
        }
    }

    return nullptr;
}