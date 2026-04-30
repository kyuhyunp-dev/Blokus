#ifndef GAME_HPP
#define GAME_HPP

#include "StatisticsTracker.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Nodes/Arena.hpp"
#include "Player.hpp"
#include "Command/CommandQueue.hpp"

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

        explicit Game(FontHolder& fonts);
        void run();

    private:
        void processEvents();
        void update(sf::Time elapsedTime);
        void render();

        void updateView(sf::Vector2u windowSize);
        void setQuery();
        void loadTextures();

    private:
        static const float PlayerSpeed;
        static const sf::Time TimePerFrame;
        
        sf::RenderWindow mWindow;
        sf::View mMainView;

        Referee mReferee;
        Arena mArena;
        Player mPlayer;
        CommandQueue mCommandQueue;

        TextureHolder mTextures;
        FontHolder &mFonts;

        StatisticsTracker mStatistics;
};

#endif