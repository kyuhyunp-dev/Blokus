#include "Nodes/BlockNode.hpp"

BlockNode::BlockNode(const sf::Texture& texture, const sf::IntRect& textureRect)
    : mBlock(texture, textureRect)
{

}

void BlockNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mBlock, states);
}