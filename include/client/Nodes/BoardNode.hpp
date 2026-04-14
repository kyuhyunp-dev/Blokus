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


private:
    virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::array<bool, TeamCount> mIsFirstMove;

    sf::RectangleShape mBackground;
    sf::VertexArray mBoardLines;

    std::array<int, TeamCount> mScores;
    std::array<Team, Blokus::BoardSize * Blokus::BoardSize> mBoard;

};
