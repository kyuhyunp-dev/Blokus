#include "PieceNode.hpp"

#include "Resource/ResourceIdentifiers.hpp"
#include "shared/Team.hpp"
#include "shared/Constants.hpp"

#include "SFML/Graphics/RectangleShape.hpp"


class BoardNode : public SceneNode
{
public:
    BoardNode(TextureHolder& textures);

    // Finalizes the piece on the board
    bool placePiece(std::unique_ptr<PieceNode> piece, int gridX, int gridY);

    sf::FloatRect getGlobalBounds() const;

private:
    // Checks if a piece can legally land at a specific grid coordinate
    bool checkPlacement(const PieceNode& piece, int gridX, int gridY) const;

    virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::array<bool, TeamCount> mIsFirstMove;

    sf::RectangleShape mBackground;
    sf::VertexArray mBoardLines;

    std::array<int, TeamCount> mScores;
    std::array<Team, Blokus::BoardSize * Blokus::BoardSize> mBoard;

};
