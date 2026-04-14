#include "PieceNode.hpp"

#include "Resource/ResourceIdentifiers.hpp"
#include "shared/Team.hpp"
#include "shared/Constants.hpp"
#include "Query/BoardQuery.hpp"

#include "SFML/Graphics/RectangleShape.hpp"


class BoardNode : public SceneNode , public BoardQuery
{
public:
    BoardNode(TextureHolder& textures);

    sf::FloatRect getGlobalBounds() const;

    sf::Vector2i getMinSnappedGrid(sf::Vector2f worldPos, sf::Vector2f minOffset) const override;

    unsigned int getCategory() const override;

    virtual void updateShadow(int pieceId, sf::Vector2i minSnappedGrid, sf::Color color);

private:
    virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

    void drawShadow(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    struct Shadow
    {
        int pieceId;
        sf::Vector2i minCoord; 
        sf::Color color;
    };

    sf::RectangleShape mBackground;
    sf::VertexArray mBoardLines;

    std::array<Team, Blokus::BoardSize * Blokus::BoardSize> mBoard;
    std::optional<Shadow> mShadow;
};
