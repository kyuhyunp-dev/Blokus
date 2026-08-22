#include "States/State.hpp"
#include "States/StateStack.hpp"
#include <SFML/Graphics/RenderWindow.hpp>


State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, 
	FontHolder& fonts, GameSessionData& gameSessionData, NetworkClient& networkClient,
	const PolyominoDefinition& library)
	: window(&window)
	, textures(&textures)
	, fonts(&fonts)
	, gameSessionData(&gameSessionData)
	, networkClient(&networkClient)
	, library(&library)
{
}

State::State(StateStack& stack, Context context)
	: mStack(&stack)
	, mContext(context)
	, mLoadedCursors()
	, mCurrentMouseCursorType(sf::Cursor::Type::Arrow)
{
	loadMouseCursor(sf::Cursor::Type::Arrow);
	loadMouseCursor(sf::Cursor::Type::Text);
	loadMouseCursor(sf::Cursor::Type::Hand);
}

State::~State()
{
}

void State::requestStackPush(States::ID stateID)
{
	mStack->pushState(stateID);
}

void State::requestStackPop()
{
	mStack->popState();
}

void State::requestStateClear()
{
	mStack->clearStates();
}

State::Context State::getContext() const
{
	return mContext;
}

void State::onActivate()
{
}

void State::onDestroy()
{
}

std::optional<sf::Vector2f> State::getWorldMousePosition(const sf::Event& event) const
{
    const auto window = getContext().window;

    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        return window->mapPixelToCoords(mouseMoved->position);
    }
    if (auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        return window->mapPixelToCoords(mousePressed->position);
    }
    if (auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>())
    {
        return window->mapPixelToCoords(mouseReleased->position);
    }

    return std::nullopt;
}

void State::loadMouseCursor(sf::Cursor::Type cursorType)
{
    auto cursor = sf::Cursor::createFromSystem(cursorType);

    if (cursor.has_value())
    {
        mLoadedCursors.emplace(cursorType, std::move(cursor.value()));
    }
}

void State::setMouseCursor(sf::Cursor::Type cursorType)
{
    if (cursorType == mCurrentMouseCursorType)
    {
        return;
    }

    auto cursor = mLoadedCursors.find(cursorType);
    if (cursor == mLoadedCursors.end())
    {
        return;
    }

    getContext().window->setMouseCursor(cursor->second);
    mCurrentMouseCursorType = cursorType;
}