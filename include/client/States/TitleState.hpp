#ifndef TITLE_STATE_HPP
#define TITLE_STATE_HPP

#include "States/State.hpp"
#include "GUI/Container.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/Graphics/RenderWindow.hpp>


class TitleState : public State
{
public:
	TitleState(StateStack& stack, Context context);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

private:
	GUI::Container mGUIContainer;

};

#endif