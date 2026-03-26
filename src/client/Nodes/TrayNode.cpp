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
    piece->setState(PieceState::Ready);

    // Store a raw pointer for quick lookup (The "Ref")
    // This allows withdrawPiece(slotId) to know WHICH child to detach.
    mSlots[slotId] = piece.get();  

    // Transfer ownership to the SceneNode hierarchy
    // This handles the 'private draw' issue automatically.
    this->attachChild(std::move(piece));
}

// The "Dumb" extraction: Just gives up ownership of the piece at slotId
// The Player class decides when to call this based on getSlotManifest()
TrayNode::PiecePtr TrayNode::withdrawPiece(int slotId)
{
    // Check if the slot actually has a piece (isOccupied logic)
    auto it = mSlots.find(slotId);
    assert(it != mSlots.end()); 

    // Remove the entry from the map (the slot is now "not occupied")
    PieceNode* target = it->second;
    mSlots.erase(it);

    auto detached = detachChild(*target);
    return PiecePtr(static_cast<PieceNode*>(detached.release()));
}

// Returns a snapshot of all slots for the Player/Controller to perform hit-tests
std::vector<SlotInfo> TrayNode::getSlotManifest() const
{
    std::vector<SlotInfo> manifest;
    manifest.reserve(Blokus::DeckSize);

    for (int id = 0; id < Blokus::DeckSize; ++id) 
    {
        auto it = mSlots.find(id);
        bool isOccupied = (it != mSlots.end());

        std::optional<int> pieceId = std::nullopt;
        std::function<bool(sf::Vector2f)> hitTest = [](sf::Vector2f) 
        { 
            return false; 
        };
 
        if (isOccupied) {
            PieceNode* piece = it->second;
            pieceId = piece->getId();

            hitTest = [piece](sf::Vector2f point) 
            { 
                return piece->contains(point); 
            };
        }
        
        manifest.push_back({ pieceId, hitTest });
    }

    return manifest;
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