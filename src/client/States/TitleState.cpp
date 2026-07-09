#include "States/TitleState.hpp"
#include "GUI/Button.hpp"
#include "GUI/TextBox.hpp"
#include "GUI/Label.hpp"
#include "Resource/ResourceHolder.hpp"
#include "ClientUtility.hpp"
#include <iostream>


TitleState::TitleState(StateStack& stack, Context context)
	: State(stack, context)
{
	const sf::Font& font = getContext().fonts->get(Fonts::Sansation);

	sf::Vector2f windowSize(context.window->getSize());
	float centerX = windowSize.x / 2.f;
	float centerY = windowSize.y / 2.f;

	// Blokem Label
	auto titleLabel = std::make_unique<GUI::Label>("BLOKEM", font, 80);
	titleLabel->setPosition(sf::Vector2f(centerX, centerY - 200.f));
	mGUIContainer.pack(std::move(titleLabel));

	// Create Match Button
	auto createButton = std::make_unique<GUI::Button>(
		"Create Match", font, sf::Vector2f(250.f, 60.f)
	);
	createButton->setPosition(sf::Vector2f(centerX - 150.f, centerY + 100.f));
	createButton->setCallback([this]() 
	{ // use network to 
		// requestCreateMatch();
		std::cout << "Create!\n";
	});
	mGUIContainer.pack(std::move(createButton));

	// Match Code Text Box
	auto codeTextBox = std::make_unique<GUI::TextBox>(
		font, sf::Vector2f(250.f, 40.f)
	);
	GUI::TextBox* codeTextBoxPtr = codeTextBox.get();
	
	codeTextBox->setPosition(sf::Vector2f(centerX + 200.f, centerY + 50.f));
	mGUIContainer.pack(std::move(codeTextBox));

	// Join Button
	auto joinButton = std::make_unique<GUI::Button>(
		"Join Match", font, sf::Vector2f(250.f, 60.f)
	);
	joinButton->setPosition(sf::Vector2f(centerX + 200.f, centerY + 100.f));
	joinButton->setCallback([this, codeTextBoxPtr]()
	{
		std::string matchCode = codeTextBoxPtr->getText();
		if (!matchCode.empty())
		{
			// requestJoinMatch(matchCode);
			std::cout << "Join!\n";
		}
	});
	mGUIContainer.pack(std::move(joinButton));
}

void TitleState::draw()
{
	getContext().window->draw(mGUIContainer);
}

bool TitleState::update(sf::Time dt)
{
	mGUIContainer.update(dt);
	return false;
}

bool TitleState::handleEvent(const sf::Event& event)
{
	auto worldMousePos = getWorldMousePosition(event);
    mGUIContainer.handleEvent(event, worldMousePos);

    if ((event.is<sf::Event::MouseMoved>() || event.is<sf::Event::MouseButtonPressed>() || event.is<sf::Event::MouseButtonReleased>()) && 
		worldMousePos)
    {
        setMouseCursor(mGUIContainer.getMouseCursorType(worldMousePos.value()).value_or(sf::Cursor::Type::Arrow));
    }

	return false;
}
