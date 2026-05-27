#ifndef TITLE_STATE_HPP
#define TITLE_STATE_HPP

#include "States/State.hpp"

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

protected:
    bool getShowText() const { return mShowText; }
    sf::Time getTextEffectTime() const { return mTextEffectTime; }

private:
	sf::Text mTitleText;
	sf::Text mText;

	bool mShowText;
	sf::Time mTextEffectTime;
};

#endif