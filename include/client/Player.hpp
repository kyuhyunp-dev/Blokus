#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "shared/Team.hpp"
#include "shared/Referee.hpp"
#include "SFML/Window/Event.hpp"
#include <map>
#include <optional>


namespace sf
{
    class RenderWindow;
}

class CommandQueue;
class TrayQuery;
class BoardQuery;
class PieceNode;

class Player
{
    public:
        Player(sf::RenderWindow& window, Referee& referee);
        
        virtual void setQuery(TrayQuery* tray, BoardQuery* board);

        virtual void setTeam(Team team);
        
        virtual void handleEvent(const sf::Event& event, CommandQueue& commands);

        virtual std::optional<int> getHeldPieceId() const;

        virtual Team getTeam() const;

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
        
        void pushPlaceCommand(sf::Vector2i minSnappedGrid, CommandQueue& commands) const;

        void pushReturnCommand(CommandQueue& commands) const;

        void pushClearShadowCommand(CommandQueue& commands) const;

        int getTransformedId(int currentId, Transformation transform) const;
        
        int getCanonicalId(int transformedId) const;
        
        void initializeKeys();

    private:
        sf::RenderWindow& mWindow;
        Referee& mReferee;

        Team mTeam = Team::None; 

        TrayQuery* mTrayPtr;
        BoardQuery* mBoardPtr;
        PieceNode* mHeldPiecePtr; 

        sf::Vector2i mCurrentMousePos;

        std::map<sf::Keyboard::Key, Transformation> mKeyBinding;
};

#endif