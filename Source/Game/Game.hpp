#ifndef GAME_GAME_HPP
#define GAME_GAME_HPP

#include "Tracker/StatisticsTracker.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>


class Game
{
    public:
        // Non Copyable
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;

        explicit Game();
        void run();

        //
        void updateStatistics(sf::Time elapsedTime);

    private:
        void processEvents();
        void update(sf::Time elapsedTime);
        void render();

        void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

    private:
        static const float PlayerSpeed;
        static const sf::Time TimePerFrame;

        sf::RenderWindow mWindow;
        sf::Texture mTexture;
        sf::Sprite mPlayer;
        sf::Font mFont;
        StatisticsTracker mStatistics;
        
        bool mIsMovingUp;
        bool mIsMovingDown;
        bool mIsMovingRight;
        bool mIsMovingLeft;
};

#endif