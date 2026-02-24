#include "Nodes/BlockNode.hpp"
#include "shared/Constants.hpp"
 

BlockNode::BlockNode(const sf::Texture& texture, const sf::IntRect& textureRect)
    : mBlock(texture, textureRect)
{
    float scaleFactor = Blokus::GridSize / Blokus::TileSize; 
    setScale({scaleFactor, scaleFactor});
}

void BlockNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const 
{
    target.draw(mBlock, states);
}