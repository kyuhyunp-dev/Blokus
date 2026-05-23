#include "Nodes/BoardNode.hpp"
#include "Nodes/BlockNode.hpp"

#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "ClientConfig.hpp"
#include <cmath>


BoardNode::BoardNode(const PolyominoDefinition& library)
    : mLibrary(library)
    , mBoardLines(sf::PrimitiveType::Lines, (Config::BoardSize + 1) * 4)
{  
    float boardSize = Config::BoardSize * Config::GridSize;
    
    // Setup Background
    mBackground.setSize({boardSize, boardSize});    
    mBackground.setFillColor(sf::Color::White); 

    sf::Color lineCol(100, 100, 100);
    int vertexCount = 0;

    // Setup Grid Lines (Vertical and Horizontal)
    for (int i = 0; i <= Config::BoardSize; ++i) 
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

const std::optional<BoardNode::Shadow> BoardNode::getShadow() const 
{
    return mShadow;
}

void BoardNode::updateShadow(int pieceId, sf::Vector2i minSnappedGrid, sf::Color color)
{
    mShadow = { pieceId, minSnappedGrid, color };
}

void BoardNode::clearShadow()
{
    mShadow = std::nullopt;
}

void BoardNode::addPiece(std::unique_ptr<PieceNode> piece, sf::Vector2i gridPos) {
    sf::Vector2f localPos = { 
        gridPos.x * Config::GridSize, 
        gridPos.y * Config::GridSize 
    };
    
    piece->setPosition(localPos);
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
    const auto &coordinates = mLibrary.polyominoById.at(mShadow->pieceId).blocks;

    sf::RectangleShape rect(sf::Vector2f(Config::GridSize, Config::GridSize));
    rect.setFillColor(mShadow->color);

    for (const auto& offset : coordinates)
    {
        sf::Vector2i gridPos = mShadow->minCoord + offset;
        if (gridPos.x >= 0 && gridPos.x < Config::BoardSize &&
            gridPos.y >= 0 && gridPos.y < Config::BoardSize)
        {
            float px = gridPos.x * Config::GridSize;
            float py = gridPos.y * Config::GridSize;
            
            rect.setPosition({px, py});
            target.draw(rect, states); 
        }
    }
}



