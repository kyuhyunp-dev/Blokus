#include "shared/Referee.hpp"

#include <cassert>

Referee::Referee()
    : library(Blokus::PolyominoGenerator::getData())
{
    for (int team = 0; team < TeamCount; ++team)
    {
        mIsFirstMove[team] = true;
        mScores[team] = 0;

        int x = (team & (1 << 0) ? Blokus::BoardSize - 1 : 0);
        int y = (team & (1 << 1) ? Blokus::BoardSize - 1 : 0);
        mStartPos[team] = { x, y };
    }

    for (int x = 0; x < Blokus::BoardSize; ++x)
    {
        for (int y = 0; y < Blokus::BoardSize; ++y)
        {
            mBoard[Blokus::getIndex(x, y)] = Team::None;
        }
    }
}

bool Referee::isValid(int pieceId, sf::Vector2i minOffset, Team team) const
{
    auto blocks = getBlockPositions(pieceId, minOffset);
    auto edges = getEdgePositions(pieceId, minOffset);
    auto corners = getCornerPositions(pieceId, minOffset);

    if (!canFit(blocks) ||
        (mIsFirstMove[team] && !isStartValid(blocks, team)) ||
        (!mIsFirstMove[team] && !hasCornerContact(corners, team)) ||
        hasEdgeContact(edges, team)) 
    {
        return false;
    }
    
    return true;
}

bool Referee::canFit(const std::vector<sf::Vector2i>& blocks) const
{
    for (const auto pos : blocks) {
        if (!isWithinBound(pos))
        {
            return false;
        }
        if (mBoard[Blokus::getIndex(pos.x, pos.y)] != Team::None) 
        {
            return false;
        }
    }

    return true;
}

bool Referee::isStartValid(const std::vector<sf::Vector2i>& blocks, Team team) const  
{
    assert(mIsFirstMove[team] && "Needs to be first move!");
    
    for (const auto pos : blocks) {
        if (pos == mStartPos[team]) 
        {
            return true;
        }
    }

    return false; 
}

bool Referee::hasCornerContact(const std::vector<sf::Vector2i>& corners, Team team) const
{
    assert(!mIsFirstMove[team] && "Must not be first move!");

    for (const auto pos : corners) 
    {
        if (isWithinBound(pos) && 
        mBoard[Blokus::getIndex(pos.x, pos.y)] == team) 
        {
            return true;
        }
    }

    return false;
}

bool Referee::hasEdgeContact(const std::vector<sf::Vector2i>& edges, Team team) const
{
    for (const auto pos : edges) 
    {
        if (isWithinBound(pos) && 
        mBoard[Blokus::getIndex(pos.x, pos.y)] == team) 
        {
            return true;
        }
    }

    return false;
}

bool Referee::isWithinBound(const sf::Vector2i& pos) const
{
    return !(pos.x < 0 || pos.x >= Blokus::BoardSize || 
        pos.y < 0 || pos.y >= Blokus::BoardSize);
}

std::vector<sf::Vector2i> Referee::getBlockPositions(int pieceId, sf::Vector2i minOffset) const
{
    std::vector<sf::Vector2i> blockPositions;
    blockPositions.reserve(Blokus::MaxBlocks);
    const auto& blocks = library.polyominoById.at(pieceId).blocks;
    for (auto pos : blocks) 
    {
        blockPositions.push_back(minOffset + pos);
    }
    return blockPositions;
}

std::vector<sf::Vector2i> Referee::getEdgePositions(int pieceId, sf::Vector2i minOffset) const
{
    std::vector<sf::Vector2i> edgePositions;
    const auto& edges = library.polyominoById.at(pieceId).sensors.edges;;
    for (auto pos : edges) 
    {
        edgePositions.push_back(minOffset + pos);
    }

    return edgePositions;
}

std::vector<sf::Vector2i> Referee::getCornerPositions(int pieceId, sf::Vector2i minOffset) const
{
    std::vector<sf::Vector2i> cornerPositions;
    const auto& corners = library.polyominoById.at(pieceId).sensors.corners;;
    for (auto pos : corners) 
    {
        cornerPositions.push_back(minOffset + pos);
    }

    return cornerPositions;
}


