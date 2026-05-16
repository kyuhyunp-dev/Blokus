#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "States/StateStack.hpp"
#include "StatisticsTracker.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>



class Application
{
public:
	Application(FontHolder& fonts);

	// Delete copy constructor and assignment operator
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void run();

protected:
	void updateView(sf::Vector2u windowSize);
	const sf::View& getMainView() const;

private:  
	void processInput();
	void update(sf::Time dt);

	void render();

	void registerStates();

private:
	static const sf::Time TimePerFrame;

	sf::RenderWindow mWindow;
	sf::View mMainView;

	TextureHolder mTextures;
	FontHolder& mFonts;

	StateStack mStateStack;
	
	StatisticsTracker mStatistics;
};

#endif
