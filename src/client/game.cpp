#include "game.hpp"
#include <stdexcept>


const float Game::kPlayerSpeed = 100.f;
const sf::Time Game::kTimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
    : window_(sf::VideoMode({ 640, 480 }), "Blokus", sf::Style::Close)
    , texture_()
    , player_(texture_)
    , font_()
    , statistics_()
    , is_moving_up_(false)
    , is_moving_down_(false)
    , is_moving_right_(false)
    , is_moving_left_(false)
{

    std::string texture_filename = std::string(ASSETS_PATH) + "/client/textures/tiles.png";
    if (!texture_.loadFromFile(texture_filename))
    {
        throw std::runtime_error("Failed to load " + texture_filename);
    }

    player_.setTexture(texture_, true);
    player_.setPosition({ 100.f, 100.f });
    player_.setScale({ 1.f / 20.f, 1.f / 20.f });
    
    std::string font_filename = std::string(ASSETS_PATH) + "/client/fonts/sansation.ttf";
    if (!font_.openFromFile(font_filename))
    {
        throw std::runtime_error("Failed to load " + font_filename);
    }
}

void Game::Run()
{
    sf::Clock clock;
    sf::Time time_since_last_update = sf::Time::Zero;
    
    while (window_.isOpen())
    {
        sf::Time elapsed_time = clock.restart();
        time_since_last_update += elapsed_time;
        
        while (time_since_last_update > kTimePerFrame)
        {
            time_since_last_update -= kTimePerFrame;
            
            ProcessEvents();
            Update(kTimePerFrame);
        }

        statistics_.Update(elapsed_time);
        Render();
    }
}

void Game::ProcessEvents()
{
    while (const std::optional event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window_.close();
            return;
        }

        if (const auto* key_pressed = 
            event->getIf<sf::Event::KeyPressed>())
        {
            HandlePlayerInput(key_pressed->code, true);
        }
        else if (const auto* key_released =
            event->getIf<sf::Event::KeyReleased>())
        {
            HandlePlayerInput(key_released->code, false);
        }
    }
}

void Game::Update(sf::Time elapsed_time)
{
    sf::Vector2f movement(0.f, 0.f);
    if (is_moving_up_)
        movement.y -= kPlayerSpeed;
    if (is_moving_down_)
        movement.y += kPlayerSpeed;
    if (is_moving_left_)
        movement.x -= kPlayerSpeed;
    if (is_moving_right_)
        movement.x += kPlayerSpeed;

    player_.move(movement * elapsed_time.asSeconds());
}

void Game::Render()
{
    window_.clear();
    window_.draw(player_);
    window_.display();
}

void Game::HandlePlayerInput(sf::Keyboard::Key key, bool is_pressed)
{
    if (key == sf::Keyboard::Key::Up)
        is_moving_up_ = is_pressed;
    else if (key == sf::Keyboard::Key::Down)
        is_moving_down_ = is_pressed;
    else if (key == sf::Keyboard::Key::Left)
        is_moving_left_ = is_pressed;
    else if (key == sf::Keyboard::Key::Right)
        is_moving_right_ = is_pressed;
}