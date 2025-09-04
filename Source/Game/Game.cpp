#include "Game/Game.hpp"
#include "Utility/StringHelpers.hpp"

#include <stdexcept>


const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f/60.f);

Game::Game()
    : mWindow(sf::VideoMode({ 640, 480 }), "Blokus", sf::Style::Close)
    , mTexture()
    , mPlayer(mTexture)
    , mFont()
    , mStatistics()
    , mIsMovingUp(false)
    , mIsMovingDown(false)
    , mIsMovingRight(false)
    , mIsMovingLeft(false)
{
    std::string textureFilename = std::string(ASSETS_DIR) + "/Textures/Tiles.png";
    if (!mTexture.loadFromFile(textureFilename))
    {
        throw std::runtime_error("Failed to load " + textureFilename);
    }

    mPlayer.setTexture(mTexture, true);
    mPlayer.setPosition({ 100.f, 100.f });
    mPlayer.setScale({ 1.f / 20.f, 1.f / 20.f });
    
    std::string fontFilename = std::string(ASSETS_DIR) + "/Fonts/Sansation.ttf";
    if (!mFont.openFromFile(fontFilename))
    {
        throw std::runtime_error("Failed to load " + fontFilename);
    }
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    
    while (mWindow.isOpen())
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            
            processEvents();
            update(TimePerFrame);
        }

        mStatistics.update(elapsedTime);
        render();
    }
}

void Game::processEvents()
{
    while (const std::optional event = mWindow.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            mWindow.close();
            return;
        }

        if (const auto* keyPressed = 
            event->getIf<sf::Event::KeyPressed>())
        {
            handlePlayerInput(keyPressed->code, true);
        }
        else if (const auto* keyReleased =
            event->getIf<sf::Event::KeyReleased>())
        {
            handlePlayerInput(keyReleased->code, false);
        }
    }
}

void Game::update(sf::Time elapsedTime)
{
    sf::Vector2f movement(0.f, 0.f);
    if (mIsMovingUp)
        movement.y -= PlayerSpeed;
    if (mIsMovingDown)
        movement.y += PlayerSpeed;
    if (mIsMovingLeft)
        movement.x -= PlayerSpeed;
    if (mIsMovingRight)
        movement.x += PlayerSpeed;

    mPlayer.move(movement * elapsedTime.asSeconds());
}

void Game::render()
{
    mWindow.clear();
    mWindow.draw(mPlayer);
    mWindow.display();
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
    if (key == sf::Keyboard::Key::Up)
        mIsMovingUp = isPressed;
    else if (key == sf::Keyboard::Key::Down)
        mIsMovingDown = isPressed;
    else if (key == sf::Keyboard::Key::Left)
        mIsMovingLeft = isPressed;
    else if (key == sf::Keyboard::Key::Right)
        mIsMovingRight = isPressed;
}