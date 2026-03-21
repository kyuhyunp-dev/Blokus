#include "Game.hpp"
#include "Path.hpp"
#include "shared/Team.hpp"
#include "Config.hpp"

#include <stdexcept>
#include <iostream>


const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);


Game::Game(FontHolder& fonts, TextureHolder& textures)
    : mWindow(sf::VideoMode({Config::ScreenWidth, Config::ScreenHeight}) , "Blokus")
    , mMainView()
    , mTextures(textures)
    , mFonts(fonts)
    , mStatistics()
    , mArena({14, 18, 17, 16, 15, 14, 13, 12, 11, 9, 10, 11, 12, 13, 14, 15, 16, 17}) 
    , mIsMovingUp(false)
    , mIsMovingDown(false)
    , mIsMovingRight(false)
    , mIsMovingLeft(false)
{
    updateView(mWindow.getSize());
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

        if (const auto* resized = 
            event->getIf<sf::Event::Resized>()) {
            updateView({resized->size});
        }

    }
}

void Game::update(sf::Time elapsedTime)
{
    mArena.update(elapsedTime);
}

void Game::updateView(sf::Vector2u windowSize) {
    float windowRatio = windowSize.x / static_cast<float>(windowSize.y);
    float viewRatio = Config::VirtualRes.x / Config::VirtualRes.y;

    sf::FloatRect viewport({0.f, 0.f}, {1.f, 1.f});

    if (windowRatio > viewRatio) 
    {
        // Window is wider than 16:9 (add side bars)
        viewport.size.x = viewRatio / windowRatio;
    } 
    else 
    {
        // Window is taller than 16:9 (add top/bottom bars)
        viewport.size.y = windowRatio / viewRatio;
    }

    mMainView.setSize(Config::VirtualRes);
    mMainView.setViewport(viewport);
    // Center the camera on our virtual world
    mMainView.setCenter(Config::VirtualRes / 2.f);
    
}

void Game::render()
{
    mWindow.clear(sf::Color::Black);
    mWindow.setView(mMainView);
    mArena.draw(mWindow, sf::RenderStates::Default);
    mWindow.display();
}


