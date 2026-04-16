#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "shared/Team.hpp"
#include "SFML/Window/Event.hpp"
#include <map>

namespace sf
{
    class RenderWindow;
}

class Referee;
class CommandQueue;
class TrayQuery;
class BoardQuery;
class PieceNode;

class Player
{
    public:
        Player(sf::RenderWindow& window);
        
        void setQuery(TrayQuery* tray, BoardQuery* board);

        void setTeam(Team team);
        
        void handleEvent(const sf::Event& event, CommandQueue& commands);

        std::optional<int> getHeldPieceId() const;

        Team getTeam() const;

    private:
        enum Transformation
        {
            RotateCW,
            RotateCCW,
            ReflectH,
            ReflectV,
            TransformationCount
        };

    private:
        void pushGrabCommand(sf::Vector2f worldPos, CommandQueue& commands) const;

        void pushShadowCommand(sf::Vector2f worldPos, CommandQueue& command) const;
        
        int getTransformedId(int currentId, Transformation transform) const;

        void initialzeKeys();

    private:
        sf::RenderWindow& mWindow;

        Team mTeam = Team::None; 

        TrayQuery* mTrayPtr;
        BoardQuery* mBoardPtr;
        PieceNode* mHeldPiecePtr; 

        std::map<sf::Keyboard::Key, Transformation> mKeyBinding;
};

#endif