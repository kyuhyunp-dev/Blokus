#ifndef MOCK_PLAYER_HPP
#define MOCK_PLAYER_HPP

#include <gmock/gmock.h>
#include "Player.hpp"
#include "shared/Team.hpp"
#include "Command/CommandQueue.hpp"

// Forward declarations to avoid heavy includes if possible
class TrayQuery;
class BoardQuery;

class MockPlayer : public Player 
{
public:
    MockPlayer(sf::RenderWindow& window, Referee& referee)
        : Player(window, referee) 
    {
    }


    MOCK_METHOD(void, handleEvent, (const sf::Event& event, CommandQueue& commands), (override));
    MOCK_METHOD(void, setTeam, (Team team), (override));
    MOCK_METHOD(void, setQuery, (TrayQuery* tray, BoardQuery* board), (override));
    MOCK_METHOD(Team, getTeam, (), (const, override));
    MOCK_METHOD(std::optional<int>, getHeldPieceId, (), (const, override));
};

#endif