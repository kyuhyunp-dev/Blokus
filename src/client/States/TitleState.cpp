#include "States/TitleState.hpp"
#include "Resource/ResourceHolder.hpp"
#include "ClientUtility.hpp"


TitleState::TitleState(StateStack& stack, Context context)
	: State(stack, context)
	, mTitleText(context.fonts->get(Fonts::ID::Sansation), "BLOKEM", 80)
	, mText(context.fonts->get(Fonts::ID::Sansation), "Press any key to start", 30)
	, mShowText(true)          
	, mTextEffectTime(sf::Time::Zero)
{
	centerOrigin(mTitleText);
	centerOrigin(mText);
	
	mText.setPosition(context.window->getView().getCenter());
	mTitleText.setPosition({mText.getPosition().x, 150.f});
}

void TitleState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.draw(mTitleText);

	if (mShowText)
	{
		window.draw(mText);
	} 

}

bool TitleState::update(sf::Time dt)
{
	mTextEffectTime += dt;

	if (mTextEffectTime >= sf::seconds(0.5f))
	{
		mShowText = !mShowText;
		mTextEffectTime = sf::Time::Zero;
	}
  
	return false;
}

bool TitleState::handleEvent(const sf::Event& event)
{
	if (event.getIf<sf::Event::KeyReleased>())
	{
		requestStackPop();
		requestStackPush(States::ID::Lobby);
	}

	return false;
}