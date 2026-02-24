#include "Game.hpp"
#include "Resource/Path.hpp"
#include "shared/Team.hpp"

#include <stdexcept>
#include <iostream>


const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);


Game::Game(FontHolder& fonts, TextureHolder& textures)
    : mWindow(sf::VideoMode({ 640, 480 }), "Blokus", sf::Style::Close)
    , mTextures(textures)
    , mFonts(fonts)
    , mStatistics()
    , mPiece(88, Team::Green, textures)
    , mIsMovingUp(false)
    , mIsMovingDown(false)
    , mIsMovingRight(false)
    , mIsMovingLeft(false)
{

    mPiece.setPosition({ 100.f, 100.f });
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

    mPiece.move(movement * elapsedTime.asSeconds());
}

void Game::render()
{
    mWindow.clear();
    mWindow.draw(mPiece);
    std::cout << mPiece.getPosition().x << ", "
    << mPiece.getPosition().y << "\n";
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