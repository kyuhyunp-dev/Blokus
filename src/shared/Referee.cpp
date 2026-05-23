#include "shared/Referee.hpp"

#include <cassert>


Referee::Referee(const PolyominoDefinition& library)
    : mLibrary(library)
{
    for (int team = 0; team < TeamCount; ++team)
    {
        mIsFirstMove[team] = true;
        mScores[team] = 0;

        int x = (team & (1 << 0) ? Config::BoardSize - 1 : 0);
        int y = (team & (1 << 1) ? Config::BoardSize - 1 : 0);
        mStartPos[team] = { x, y };
    }

    for (int cell = 0; cell < Config::CellCount; ++cell)
    {
        mBoard[cell] = Team::None;
    }
}

void Referee::place(int pieceId, sf::Vector2i minOffset, Team team)
{
    assert(isValid(pieceId, minOffset, team));

    auto blocks = getBlockPositions(pieceId, minOffset);
    for (const auto pos : blocks)
    {
        mBoard[getIndex(pos.x, pos.y)] = team;
    }

    mIsFirstMove[team] = false;
    mScores[team] += blocks.size();
}

bool Referee::isValid(int pieceId, sf::Vector2i minOffset, Team team) const
{
    auto blocks = getBlockPositions(pieceId, minOffset);
    auto edges = getEdgePositions(pieceId, minOffset);
    auto corners = getCornerPositions(pieceId, minOffset);

    if (!canFit(blocks)) 
    {
        return false;
    }
    else if (mIsFirstMove[team]) 
    {
        return isStartValid(blocks, team);
    }
    else 
    {
        return hasCornerContact(corners, team) && !hasEdgeContact(edges, team);
    }
}

bool Referee::canFit(const std::vector<sf::Vector2i>& blocks) const
{
    for (const auto pos : blocks) {
        if (!isWithinBound(pos))
        {
            return false;
        }
        if (mBoard[getIndex(pos.x, pos.y)] != Team::None) 
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
        mBoard[getIndex(pos.x, pos.y)] == team) 
        {
            return true;
        }
    }

    return false;
}

bool Referee::hasEdgeContact(const std::vector<sf::Vector2i>& edges, Team team) const
{
    assert(!mIsFirstMove[team] && "Must not be first move!");

    for (const auto pos : edges) 
    {
        if (isWithinBound(pos) && 
        mBoard[getIndex(pos.x, pos.y)] == team) 
        {
            return true;
        }
    }

    return false;
}

bool Referee::isWithinBound(const sf::Vector2i& pos) const
{
    return pos.x >= 0 && pos.x < Config::BoardSize && 
        pos.y >= 0 && pos.y < Config::BoardSize;
}

std::vector<sf::Vector2i> Referee::getBlockPositions(int pieceId, sf::Vector2i minOffset) const
{
    std::vector<sf::Vector2i> blockPositions;
    blockPositions.reserve(Config::MaxBlocks);
    const auto& blocks = mLibrary.polyominoById.at(pieceId).blocks;
    for (auto pos : blocks) 
    {
        blockPositions.push_back(minOffset + pos);
    }
    return blockPositions;
}

std::vector<sf::Vector2i> Referee::getEdgePositions(int pieceId, sf::Vector2i minOffset) const
{
    std::vector<sf::Vector2i> edgePositions;
    const auto& edges = mLibrary.polyominoById.at(pieceId).sensors.edges;
    for (auto pos : edges) 
    {
        edgePositions.push_back(minOffset + pos);
    }

    return edgePositions;
}

std::vector<sf::Vector2i> Referee::getCornerPositions(int pieceId, sf::Vector2i minOffset) const
{
    std::vector<sf::Vector2i> cornerPositions;
    const auto& corners = mLibrary.polyominoById.at(pieceId).sensors.corners;
    for (auto pos : corners) 
    {
        cornerPositions.push_back(minOffset + pos);
    }

    return cornerPositions;
}


