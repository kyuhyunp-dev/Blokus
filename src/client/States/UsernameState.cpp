#include "States/UsernameState.hpp"
#include "GUI/Label.hpp"
#include "GUI/Button.hpp"
#include "GUI/TextBox.hpp"
#include "States/LobbyState.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Resource/ResourceHolder.hpp" 
#include <SFML/Graphics/RenderWindow.hpp>
#include <spdlog/spdlog.h>


UsernameState::UsernameState(StateStack& stack, Context context)
    : State(stack, context)
    , mGUIContainer()
    , mUsername()
{
    sf::Vector2f windowSize = context.window->getView().getSize();
    const sf::Font& font = context.fonts->get(Fonts::ID::Sansation);

    // Username TextBox
    auto username = std::make_unique<GUI::TextBox>("Enter Username", font, sf::Vector2f({ 200.f, 50.f }));
    sf::FloatRect usernameBounds = username->getGlobalBounds();
    username->setOrigin(usernameBounds.size / 2.f);
    username->setPosition({ windowSize.x / 2.f, windowSize.y / 2.f - 25.f });
    GUI::TextBox* usernamePtr = username.get();

    // Join Button 
    auto joinGame = std::make_unique<GUI::Button>("Join Game", font, sf::Vector2f({ 200.f, 50.f }));
    sf::FloatRect joinBounds = joinGame->getGlobalBounds();
    joinGame->setOrigin(joinBounds.size / 2.f);
    joinGame->setPosition({ windowSize.x / 2.f, windowSize.y / 2.f + 50.f });
    joinGame->setCallback([this, usernamePtr]()
    {
        mUsername = usernamePtr->getText();
        if (!mUsername.empty())
        {
            spdlog::info("Saving username: {}", mUsername); 
            requestStackPop();
            requestStackPush(States::ID::Lobby);
        }
        else
        {
            spdlog::warn("Username is empty! Please enter a name");
        }
    });

    // Leave Button 
    auto leave = std::make_unique<GUI::Button>("Leave", font, sf::Vector2f({ 100.f, 40.f }));
    leave->setPosition({ windowSize.x - 120.f, 20.f });
    leave->setCallback([this]()
    {
        requestStackPop();
        requestStackPush(States::ID::Title);
    });

    mGUIContainer.pack(std::move(username));
    mGUIContainer.pack(std::move(joinGame));
    mGUIContainer.pack(std::move(leave));
}

void UsernameState::draw()
{
    getContext().window->draw(mGUIContainer);
}

bool UsernameState::update(sf::Time dt)
{
	mGUIContainer.update(dt);
    return false;
}

bool UsernameState::handleEvent(const sf::Event& event)
{
    auto worldMousePos = getWorldMousePosition(event);
    mGUIContainer.handleEvent(event, worldMousePos); 

    if (event.getIf<sf::Event::MouseMoved>() && worldMousePos.has_value())
    {
        setMouseCursor(mGUIContainer.getMouseCursorType(worldMousePos.value()).value_or(sf::Cursor::Type::Arrow));
    }

    return false;
}
