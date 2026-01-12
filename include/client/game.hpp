#ifndef GAME_HPP
#define GAME_HPP

#include "statistics_tracker.hpp"

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
        void Run();

        //
        void UpdateStatistics(sf::Time elapsed_time);

    private:
        void ProcessEvents();
        void Update(sf::Time elapsed_time);
        void Render();

        void HandlePlayerInput(sf::Keyboard::Key key, bool is_pressed);

    private:
        static const float kPlayerSpeed;
        static const sf::Time kTimePerFrame;

        sf::RenderWindow window_;
        sf::Texture texture_;
        sf::Sprite player_;
        sf::Font font_;
        StatisticsTracker statistics_;
        
        bool is_moving_up_;
        bool is_moving_down_;
        bool is_moving_right_;
        bool is_moving_left_;
};

#endif