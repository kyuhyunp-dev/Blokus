#include "Nodes/BoardNode.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "Config.hpp"


BoardNode::BoardNode(TextureHolder& textures)
: mBoardLines(sf::PrimitiveType::Lines, (Blokus::BoardSize + 1) * 4)
{ // Should use private functions to organize code FirstMove, grid, background, score etc.
    float boardSize = Blokus::BoardSize * Config::GridSize;
    
    // 1. Setup Background
    mBackground.setSize({boardSize, boardSize});
    mBackground.setFillColor(sf::Color::White); 

    sf::Color lineCol(100, 100, 100);
    int vertexCount = 0;

    // 2. Setup Grid Lines (Vertical and Horizontal)
    for (int i = 0; i <= Blokus::BoardSize; ++i) 
    {
        float pos = i * Config::GridSize;
        
        // Vertical line
        mBoardLines[vertexCount++] = {{pos, 0.f}, lineCol};
        mBoardLines[vertexCount++] = {{pos, boardSize}, lineCol};
        
        // Horizontal line
        mBoardLines[vertexCount++] = {{0.f, pos}, sf::Color(100, 100, 100)};
        mBoardLines[vertexCount++] = {{boardSize, pos}, sf::Color(100, 100, 100)};
    }
}

sf::FloatRect BoardNode::getGlobalBounds() const
{
    return getTransform().transformRect(mBackground.getGlobalBounds());
}

bool BoardNode::placePiece(std::unique_ptr<PieceNode> piece, int gridX, int gridY)
{ // TODO
    return checkPlacement(*piece, gridX, gridY);
}

bool BoardNode::checkPlacement(const PieceNode& piece, int gridX, int gridY) const
{ // TODO
    return false;
}

void BoardNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Draw background
    target.draw(mBackground, states);

    // Draw grid lines
    target.draw(mBoardLines, states);

    // Shadow piece is drawn by Arena's Interaction layer, not here
    // This keeps shared visuals (Board, Tray) separate from player-specific (InteractionNode)
}

