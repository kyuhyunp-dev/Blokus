#include "Game.hpp"
#include "Path.hpp"
#include "shared/Team.hpp"
#include "Config.hpp"
#include "Nodes/TrayNode.hpp"
#include "Nodes/BoardNode.hpp"

#include <stdexcept>


const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

// Precondition: must be unique
const std::array<int, Blokus::DeckSize> deck = 
{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}; 


Game::Game(FontHolder& fonts)
    : mWindow(sf::VideoMode({Config::ScreenWidth, Config::ScreenHeight}) , "Blokus")
    , mMainView()
    , mTextures()
    , mFonts(fonts)
    , mStatistics()
    , mArena(mWindow, mTextures, deck, mCommandQueue, Team::Red) 
    , mPlayer(mWindow)
{
    updateView(mWindow.getSize());
    loadTextures();
    mArena.buildScene();
    setQuery();    

    mPlayer.setTeam(Team::Red);
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

        mPlayer.handleEvent(event.value(), mCommandQueue);
    }
}

void Game::update(sf::Time elapsedTime)
{
    while (!mCommandQueue.isEmpty())
    {
        // Arena receives the command and tells all its children (Tray, Board, ActiveNode)
        mArena.onCommand(mCommandQueue.pop(), elapsedTime);
    }

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
    mWindow.draw(mArena);   
    mWindow.display();
}

void Game::setQuery()
{
    assert(dynamic_cast<TrayQuery*>(mArena.getTrayNode()) != nullptr);
    TrayQuery* trayPtr = static_cast<TrayQuery*>(mArena.getTrayNode());

    assert(dynamic_cast<BoardQuery*>(mArena.getBoardNode()) != nullptr);
    BoardQuery* boardPtr = static_cast<BoardQuery*>(mArena.getBoardNode());

    mPlayer.setQuery(trayPtr, boardPtr);
}

void Game::loadTextures()
{
    std::string tileFile = getAssetPath("client/textures/tiles.png");
    mTextures.load(Textures::ID::Tiles, tileFile); 
}
