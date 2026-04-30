#include "Nodes/BoardNode.hpp"
#include "Nodes/BlockNode.hpp"

#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "Config.hpp"
#include <iostream>


BoardNode::BoardNode()
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

sf::Vector2i BoardNode::getMinSnappedGrid(sf::Vector2f worldPos, sf::Vector2f minOffset) const
{
    sf::Vector2f boardLocalPos = getInverseTransform().transformPoint(worldPos);
    sf::Vector2f localTopLeft = boardLocalPos - minOffset;

    sf::Vector2i minSnappedGrid;
    minSnappedGrid.x = static_cast<int>(std::round(localTopLeft.x / Config::GridSize));
    minSnappedGrid.y = static_cast<int>(std::round(localTopLeft.y / Config::GridSize));

    return minSnappedGrid;
}

unsigned int BoardNode::getCategory() const
{
    return Category::Board;
}

void BoardNode::updateShadow(int pieceId, sf::Vector2i minSnappedGrid, sf::Color color)
{
    mShadow = { pieceId, minSnappedGrid, color };
}

void BoardNode::addPiece(std::unique_ptr<PieceNode> piece, sf::Vector2i gridPos) {
    // 1. Calculate the local position relative to the BoardNode
    sf::Vector2f localPos = { 
        gridPos.x * Config::GridSize, 
        gridPos.y * Config::GridSize 
    };
    
    // 2. Set the position relative to THIS node (BoardNode)
    piece->setPosition(localPos);
    
    // 3. Attach
    attachChild(std::move(piece));
}

void BoardNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mBackground, states);

    target.draw(mBoardLines, states);
     
    if (mShadow) 
    {
        drawShadow(target, states);
    }
}   

void BoardNode::drawShadow(sf::RenderTarget& target, sf::RenderStates states) const
{
    const auto &library = Blokus::PolyominoGenerator::getData();
    const auto &coordinates = library.polyominoById.at(mShadow->pieceId).blocks;

    sf::RectangleShape rect(sf::Vector2f(Config::GridSize, Config::GridSize));
    rect.setFillColor(mShadow->color);

    for (const auto& offset : coordinates)
    {
        sf::Vector2i gridPos = mShadow->minCoord + offset;
        if (gridPos.x >= 0 && gridPos.x < Blokus::BoardSize &&
            gridPos.y >= 0 && gridPos.y < Blokus::BoardSize)
        {
            float px = gridPos.x * Config::GridSize;
            float py = gridPos.y * Config::GridSize;
            
            rect.setPosition({px, py});
            target.draw(rect, states); 
        }
    }
}



