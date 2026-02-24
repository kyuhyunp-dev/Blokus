#ifndef GAME_HPP
#define GAME_HPP

#include "StatisticsTracker.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Nodes/PieceNode.hpp"

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

        explicit Game(FontHolder& fonts, TextureHolder& textures);
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

        PieceNode mPiece;

        TextureHolder &mTextures;
        FontHolder &mFonts;

        StatisticsTracker mStatistics;
        
        bool mIsMovingUp;
        bool mIsMovingDown;
        bool mIsMovingRight;
        bool mIsMovingLeft;
};

#endif