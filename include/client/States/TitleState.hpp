#ifndef TITLE_STATE_HPP
#define TITLE_STATE_HPP

#include "States/State.hpp"
#include "GUI/Container.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include <future>
#include <chrono>


namespace GUI
{
	class Label;
}

class TitleState : public State
{
public:
	TitleState(StateStack& stack, Context context);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

protected:
	void requestCreateMatch();
	void requestJoinMatch(const std::string& matchCode);
	void processPacket(sf::Packet& packet);

	GUI::Label* mStatusLabel;

private:
	static constexpr int CODE_SIZE = 4;

	GUI::Container mGUIContainer;
	std::future<bool> mConnectionFuture;
	bool mIsConnecting = false;
};

#endif