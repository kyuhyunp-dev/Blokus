#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "SFML/Window/Event.hpp"
#include <map>

namespace sf
{
    class RenderWindow;
}

class Referee;
class CommandQueue;
class TrayQuery;

class Player
{
    public:
        enum Transformation
        {
            RotateCW,
            RotateCCW,
            FlipH,
            FlipV,
            TransformationCount
        };

    public:
        Player(sf::RenderWindow& window);
        
        void setQuery(TrayQuery* tray);

        void handleEvent(const sf::Event& event, CommandQueue& commands);

        std::optional<int> getHeldPieceId() const;

    private:
        void pushGrabCommand(int id, sf::Vector2f worldPos, CommandQueue& commands);

    private:
        sf::RenderWindow& mWindow;

        TrayQuery* mTray;
      
        std::optional<int> mHeldPieceId; // used as a state
        std::map<sf::Keyboard::Key, Transformation> mKeyBinding;
};

#endif