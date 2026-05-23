#ifndef BOARDNODE_HPP
#define BOARDNODE_HPP

#include "PieceNode.hpp"

#include "Resource/ResourceIdentifiers.hpp"
#include "shared/Team.hpp"
#include "shared/SharedConfig.hpp"
#include "Query/BoardQuery.hpp"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"


class BoardNode : public SceneNode , public BoardQuery
{
    friend class BoardNodeTest; 

public:
    struct Shadow
    {
        int pieceId;
        sf::Vector2i minCoord; 
        sf::Color color;
    };

public:
    BoardNode(const PolyominoDefinition& library);

    sf::FloatRect getGlobalBounds() const;

    sf::Vector2i getMinSnappedGrid(sf::Vector2f worldPos, sf::Vector2f minOffset) const override;

    unsigned int getCategory() const override;

    const std::optional<Shadow> getShadow() const;

    virtual void updateShadow(int pieceId, sf::Vector2i minSnappedGrid, sf::Color color);

    virtual void clearShadow();

    void addPiece(std::unique_ptr<PieceNode> piece, sf::Vector2i gridPos);
 
private:
    virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
    
    void drawShadow(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    const PolyominoDefinition& mLibrary;

    sf::RectangleShape mBackground;
    sf::VertexArray mBoardLines;

    std::optional<Shadow> mShadow;
};

#endif