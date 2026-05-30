#include "States/LobbyState.hpp"
#include "Resource/ResourceHolder.hpp"
#include "ClientUtility.hpp" // Assuming you have centerOrigin here
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>


LobbyState::LobbyState(StateStack& stack, Context context)
    : State(stack, context)
    , mTitleText(context.fonts->get(Fonts::ID::Sansation), "LOBBY", 80)
    , mStatusText(context.fonts->get(Fonts::ID::Sansation), "Waiting for players...", 30)
{
    centerOrigin(mTitleText);
    centerOrigin(mStatusText);
    
    auto windowSize = context.window->getView().getSize();
    mTitleText.setPosition({windowSize.x / 2.f, 150.f});
    mStatusText.setPosition({windowSize.x / 2.f, 250.f});
}

bool LobbyState::update(sf::Time dt)
{
    return false; 
}

void LobbyState::addPlayer(int id, const std::string& username)
{
    GameSessionData& session = *getContext().gameSessionData;
    session.playerById[id] = PlayerInfo{id, Team::Red, username, true, 0.f};
    session.match.currentPlayerCount = session.playerById.size();

    sf::Text userText(getContext().fonts->get(Fonts::ID::Sansation), username, 20);

    mUsernameTexts.push_back({id, userText});
    recalculateLayout();
}

void LobbyState::removePlayer(int id)
{
    GameSessionData& session = *getContext().gameSessionData;
    session.playerById.erase(id);
    session.match.currentPlayerCount = session.playerById.size();

    mUsernameTexts.erase(
        std::remove_if(mUsernameTexts.begin(), mUsernameTexts.end(),
        [id](const LobbySlot& slot) {
            return slot.id == id;
        }), 
        mUsernameTexts.end()
    );

    recalculateLayout();
}

void LobbyState::draw()
{
    sf::RenderWindow& window = *getContext().window;
    
    window.draw(mTitleText);
    window.draw(mStatusText);

    for (const auto& slot : mUsernameTexts) {
        window.draw(slot.text);
    }
}

bool LobbyState::handleEvent(const sf::Event& event)
{
    if (auto keyPressed = event.getIf<sf::Event::KeyReleased>()) 
    {
        if (keyPressed->code == sf::Keyboard::Key::Escape) 
        {
            getContext().networkClient->disconnect();

            requestStackPop();
            requestStackPush(States::ID::Title);
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter) 
        {
            requestStackPop();
            requestStackPush(States::ID::NetworkGame);
        }
    }

    return false;
}

const sf::Text& LobbyState::getTitleText() const
{
    return mTitleText;
}

const sf::Text& LobbyState::getStatusText() const
{
    return mStatusText;
}

const std::vector<LobbyState::LobbySlot>& LobbyState::getUsernameTexts() const
{
    return mUsernameTexts;
}

void LobbyState::recalculateLayout()
{
    for (size_t i = 0; i < mUsernameTexts.size(); ++i)
    {
        mUsernameTexts[i].text.setPosition(getOffset(i + 1));
    }
}
const sf::Vector2f LobbyState::getOffset(int pos) const
{
    return {500.f, 250.f + (pos * 150.f)};
}