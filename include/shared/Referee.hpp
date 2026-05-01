#ifndef REFEREE_HPP
#define REFEREE_HPP

#include "shared/Team.hpp"
#include "shared/Constants.hpp"
#include "shared/PolyominoUtil.hpp"

#include <vector>


class Referee 
{   
public:
    Referee();

    virtual ~Referee() = default;

    virtual void place(int pieceId, sf::Vector2i minOffset, Team team);

    virtual bool isValid(int pieceId, sf::Vector2i minOffset, Team team) const;

private:
    bool canFit(const std::vector<sf::Vector2i>& blocks) const;

    bool isStartValid(const std::vector<sf::Vector2i>& blocks, Team team) const;

    bool hasCornerContact(const std::vector<sf::Vector2i>& corners, Team team) const;

    bool hasEdgeContact(const std::vector<sf::Vector2i>& edges, Team team) const;

    bool isWithinBound(const sf::Vector2i& pos) const;

    std::vector<sf::Vector2i> getBlockPositions(int pieceId, sf::Vector2i minOffset) const;
      
    std::vector<sf::Vector2i> getEdgePositions(int pieceId, sf::Vector2i minOffset) const;
   
    std::vector<sf::Vector2i> getCornerPositions(int pieceId, sf::Vector2i minOffset) const;

private:
    const Blokus::PolyominoDefinition& library;
    
    std::array<bool, TeamCount> mIsFirstMove;
    std::array<sf::Vector2i, TeamCount> mStartPos;
    std::array<int, TeamCount> mScores;
    std::array<Team, Blokus::CellCount> mBoard;
};


#endif