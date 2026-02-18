#ifndef BLOCKNODE_HPP
#define BLOCKNODE_HPP

#include "Nodes/SceneNode.hpp"
#include <SFML/Graphics/Sprite.hpp>

class BlockNode : public SceneNode {
public:
    explicit BlockNode(const sf::Texture& texture, const sf::IntRect& textureRect);

private:
    virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
    
private:
    sf::Sprite mBlock;
};

#endif