#ifndef USERNAME_STATE_HPP
#define USERNAME_STATE_HPP


#include "States/State.hpp"
#include "GUI/Container.hpp"


class UsernameState : public State
{
public:
	UsernameState(StateStack& stack, Context context);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

private:
	GUI::Container mGUIContainer;
	std::string mUsername;
};
#endif