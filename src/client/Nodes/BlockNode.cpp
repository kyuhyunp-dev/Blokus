#include "Nodes/BlockNode.hpp"
#include "shared/Constants.hpp"
#include "Config.hpp"


BlockNode::BlockNode(const sf::Texture& texture, const sf::IntRect& textureRect)
    : mBlock(texture, textureRect)
{
    float scaleFactor = Config::GridSize / Config::TileSize; 
    setScale({scaleFactor, scaleFactor});
}

void BlockNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const 
{
    target.draw(mBlock, states);
}

bool BlockNode::contains(sf::Vector2f worldPoint) const 
{
    // 1. Get the sprite's bounds in the BlockNode's local space
    sf::FloatRect localBounds = mBlock.getLocalBounds();

    // 2. Transform that rect by the BlockNode's position/scale/rotation
    // This moves the "hitbox" to the correct spot in the PieceNode
    sf::FloatRect transformedBounds = getTransform().transformRect(localBounds);

    // 3. Since the PieceNode is at {0,0} in your test, 
    // transformedBounds is now in World Coordinates.
    return transformedBounds.contains(worldPoint);
}