#include "Application.hpp"
#include "States/TitleState.hpp"
#include "States/LobbyState.hpp"
#include "States/NetworkGameState.hpp"
#include "ClientConfig.hpp"
#include "States/State.hpp"
#include "States/StateIdentifiers.hpp"
#include "Path.hpp"

#include <iostream>


const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application::Application(TextureHolder& textures, FontHolder& fonts, const PolyominoDefinition& library)
	: mWindow(sf::VideoMode({ Config::ScreenWidth, Config::ScreenHeight }), "Blokem")
	, mMainView()
	, mTextures(textures)
	, mFonts(fonts)
	, mGameSessionData()
	, mNetworkClient()
	, mLibrary(library)
	, mStateStack(State::Context(mWindow, mTextures, mFonts, mGameSessionData, mNetworkClient, mLibrary))
	, mStatistics()
{
	mWindow.setKeyRepeatEnabled(false);
	
	mTextures.load(Textures::ID::TitleScreen, "client/textures/titlePage.png");

	updateView(mWindow.getSize());
	registerStates();
	mStateStack.pushState(States::ID::Title);
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time dt = clock.restart();
		timeSinceLastUpdate += dt;

		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processInput();
			update(TimePerFrame);

			if (mStateStack.isEmpty())
			{
				mWindow.close();
				break;
			}
		}

		if (!mWindow.isOpen())
		{
			break; 
		}

		mStatistics.update(dt);
		render();
	}
}

void Application::processInput()
{
	while (std::optional<sf::Event> event = mWindow.pollEvent())
	{
		if (!event.has_value())
		{
			continue;
		}

		if (event->is<sf::Event::Closed>())
		{
			mWindow.close();
		}
		else if (const auto* resized = 
			event->getIf<sf::Event::Resized>()) {
            updateView({resized->size});
        }
		
		mStateStack.handleEvent(event.value());
	}
}

void Application::update(sf::Time dt)
{
	mStateStack.update(dt);
    mStatistics.update(dt);
}

void Application::updateView(sf::Vector2u windowSize) 
{
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

const sf::View& Application::getMainView() const
{
	return mMainView;
}

void Application::render()
{
	mWindow.clear();
	mWindow.setView(mMainView);

	mStateStack.draw();
	mWindow.display();
}


void Application::registerStates()
{
	mStateStack.registerState<TitleState>(States::ID::Title);
	mStateStack.registerState<LobbyState>(States::ID::Lobby);
	mStateStack.registerState<NetworkGameState>(States::ID::NetworkGame);
}