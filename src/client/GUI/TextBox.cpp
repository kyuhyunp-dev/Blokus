#include "GUI/TextBox.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Clipboard.hpp>

#include <algorithm>


GUI::TextBox::TextBox(const sf::Font& font, const sf::Vector2f& size)
    : mShape(size)
    , mText(font, "", 16)
    , mShowCursor(false)
    , mCursorIndex(0)
    , mCursorTime(sf::Time::Zero)
    , mRepeatAction(RepeatAction::None)
    , mRepeatTime(sf::Time::Zero)
    , mInInitialDelay(false)
    , mSelectionStart(std::nullopt)
    , mSelectionEnd(std::nullopt)
    , mCharacterLimit(10) 
{
    mShape.setFillColor(sf::Color::White);
    mText.setFillColor(sf::Color::Black);
    
    mCursor.setSize({ 2.f, 18.f }); 
    mCursor.setFillColor(sf::Color::Black);
    
    mText.setString("A");
    sf::FloatRect sampleBounds = mText.getLocalBounds();
    float fixedCenterY = (size.y - sampleBounds.size.y) / 2.0f - sampleBounds.position.y;

    mCursor.setPosition({ size.x / 2.0f, size.y / 2.0f - mCursor.getSize().y / 2.0f });

    mText.setPosition({ 0.f, fixedCenterY });
    mText.setString("");

    mSelectionHighlight.setFillColor(sf::Color(173, 216, 230, 128));

    updateLayout();
}

bool GUI::TextBox::isInteractive() const 
{
    return true;
}

void GUI::TextBox::press(std::optional<sf::Vector2f> worldMousePos)
{
    Component::press(worldMousePos);

    if (worldMousePos)
    {
        assert(getGlobalBounds().contains(*worldMousePos));

        std::size_t index = getCharacterIndexAt(*worldMousePos); 
        setCursorPosition(index);
   }
}

void GUI::TextBox::release(std::optional<sf::Vector2f> worldMousePos)
{
    Component::release(worldMousePos);
    
    if (worldMousePos)
    {
        assert(getGlobalBounds().contains(*worldMousePos));

        // For click without drag
        std::size_t index = getCharacterIndexAt(*worldMousePos); 
        setSelectionEnd(index); 

        if (hasHighlight())
        {
            assert(!mShowCursor);
        }
    }
}

void GUI::TextBox::unfocus()	
{
    Component::unfocus();
    
    // Clear the selection states completely
    mSelectionStart = std::nullopt;
    mSelectionEnd = std::nullopt;
    
    mShowCursor = false;
    updateHighlight();
}

void GUI::TextBox::handleEvent(const sf::Event& event, std::optional<sf::Vector2f> worldMousePos) 
{
    if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    { // If not pressed or worldMousePos is not provided, hover should handle mouse movement
        assert(isPressed());
        assert(worldMousePos);

        std::size_t index = getCharacterIndexAt(*worldMousePos);
        setSelectionEnd(index);
    }

    else if (auto keyPress = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPress->code == sf::Keyboard::Key::Backspace ||
            keyPress->code == sf::Keyboard::Key::Left ||
            keyPress->code == sf::Keyboard::Key::Right)
        {
            mInInitialDelay = true;
            mRepeatTime = sf::Time::Zero;
        }

        if (keyPress->code == sf::Keyboard::Key::Backspace)
        {
            mRepeatAction = RepeatAction::Backspace;
            executeBackspace();
        }
        else if (keyPress->code == sf::Keyboard::Key::Left)
        {
            mRepeatAction = RepeatAction::MoveLeft;
            executeMoveLeft();
        }
        else if (keyPress->code == sf::Keyboard::Key::Right)
        {
            mRepeatAction = RepeatAction::MoveRight;
            executeMoveRight();
        }
    }
    else if (auto keyRelease = event.getIf<sf::Event::KeyReleased>())
    {
        if ((keyRelease->code == sf::Keyboard::Key::Backspace && mRepeatAction == RepeatAction::Backspace) ||
            (keyRelease->code == sf::Keyboard::Key::Left && mRepeatAction == RepeatAction::MoveLeft) ||
            (keyRelease->code == sf::Keyboard::Key::Right && mRepeatAction == RepeatAction::MoveRight))
        {
            mRepeatAction = RepeatAction::None;
        }

        else if (keyRelease->code == sf::Keyboard::Key::Enter)
        {
            unfocus();
        }
    }
    else if (auto text = event.getIf<sf::Event::TextEntered>()) 
    {
        uint32_t unicode = text->unicode;

        if (unicode >= 32 && unicode < 127) 
        {
            if (mString.size() < mCharacterLimit) 
            {
                deleteSelectedText();

                mString.insert(mCursorIndex, 1, static_cast<char>(unicode));
                updateString(mString, mCursorIndex + 1);
            }
        }
    }
}

void GUI::TextBox::update(sf::Time dt)
{
    if (isPressed() && mRepeatAction == RepeatAction::None)
    { // Don't blink when mouse is pressed
        return;
    }
    if (!isFocused())
    {
        return;
    }
    if (hasHighlight())
    { 
        assert(!mShowCursor);
        return;
    }

    // Handle cursor blinking safely away from the draw call
    mCursorTime += dt;
    if (mCursorTime >= sf::seconds(0.5f))
    {
        mShowCursor = !mShowCursor;
        mCursorTime = sf::Time::Zero;
    }
   
    mRepeatTime += dt;    

    // Repeat Action
    if (mInInitialDelay)
    {
        if (mRepeatTime >= sf::milliseconds(260))
        {
            mInInitialDelay = false;
            mRepeatTime = sf::Time::Zero;
        }
    }
    else
    {
        if  (mRepeatTime >= sf::milliseconds(40))
        {
            mRepeatTime = sf::Time::Zero;

            if (mRepeatAction == RepeatAction::Backspace)
            {
                executeBackspace();
            }
            else if (mRepeatAction == RepeatAction::MoveLeft)
            {
                executeMoveLeft();
            }
            else if (mRepeatAction == RepeatAction::MoveRight)
            {
                executeMoveRight();
            }
        }
    }
}    

sf::FloatRect GUI::TextBox::getGlobalBounds() const 
{
    return getTransform().transformRect(mShape.getLocalBounds());
}

std::optional<sf::Cursor::Type> GUI::TextBox::getMouseCursorType() const
{
    if (isPressed() || isHovered())
    {
        return sf::Cursor::Type::Text;
    }
    
    return std::nullopt;
}

std::string GUI::TextBox::getText() const 
{
    return mString;
}

void GUI::TextBox::updateHighlight()
{
    if (!hasHighlight())
    {
        mSelectionHighlight.setSize({0.f, 0.f}); 
        return;
    }

    assert(!mShowCursor);

    std::size_t first = std::min(*mSelectionStart, *mSelectionEnd);
    std::size_t last = std::max(*mSelectionStart, *mSelectionEnd);

    // Find local X coordinates of the bounding characters
    float startX = mText.findCharacterPos(first).x;
    float endX = mText.findCharacterPos(last).x;

    float padding = 7.f;
    mSelectionHighlight.setPosition({ startX, padding }); 
    mSelectionHighlight.setSize({ endX - startX, mShape.getSize().y - 2 * padding });
}

bool GUI::TextBox::deleteSelectedText()
{
    if (!hasHighlight())
    {
        return false; 
    }

    assert(!mShowCursor);

    std::size_t first = std::min(*mSelectionStart, *mSelectionEnd);
    std::size_t last = std::max(*mSelectionStart, *mSelectionEnd);
    std::size_t length = last - first;

    updateString(mString.erase(first, length), first);
    return true; 
}

void GUI::TextBox::updateLayout()
{
    if (mString.empty())
    {
        mText.setString("Enter 4 Digit Code");
        mText.setFillColor(sf::Color(150, 150, 150)); 
    }
    else
    {
        mText.setString(mString);
        mText.setFillColor(sf::Color::Black); 
    }

    sf::FloatRect textBounds = mText.getLocalBounds();
    sf::Vector2f boxSize = mShape.getSize();

    float startX = (boxSize.x - textBounds.size.x) / 2.0f;
    mText.setPosition({ startX, mText.getPosition().y });

    float cursorX = 0.0f;
    if (mString.empty())
    {
        cursorX = boxSize.x / 2.0f;
    }
    else
    {
        cursorX = mText.findCharacterPos(mCursorIndex).x;
    }

    mCursor.setPosition({ cursorX, mCursor.getPosition().y });
}


std::size_t GUI::TextBox::getCharacterIndexAt(sf::Vector2f worldMousePos) const
{
    sf::FloatRect textBoxBounds = getGlobalBounds();
    if (worldMousePos.y < textBoxBounds.position.y)
    { // Case 1: mouse is above the box, 0 returned
        return 0;
    }
    else if (worldMousePos.y > textBoxBounds.position.y + textBoxBounds.size.y)
    { // Case 2: mouse is below the box, length returned
        return mString.size();
    }

    sf::Vector2f localMousePos = getInverseTransform().transformPoint(worldMousePos);
    float mouseX = localMousePos.x;

    std::size_t bestIndex = 0;
    float minDistance = std::numeric_limits<float>::max();

    for (std::size_t i = 0; i <= mString.size(); ++i)
    {
        float charX = mText.findCharacterPos(i).x;
        float distance = std::abs(charX - mouseX);

        if (distance < minDistance)
        {
            minDistance = distance;
            bestIndex = i;
        }
        else
        { // If the distance grows, we passed the closest character
            break;
        }
    }

    return bestIndex;
}

bool GUI::TextBox::hasHighlight() const
{
    return mSelectionStart && mSelectionEnd && 
    *mSelectionStart != *mSelectionEnd;
}

void GUI::TextBox::resetCursorBlink()
{
    if (hasHighlight())
    {
        assert(!mShowCursor);
        return;
    }

    mShowCursor = true;
    mCursorTime = sf::Time::Zero;
}

void GUI::TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
    states.transform *= getTransform();

    target.draw(mShape, states);
    target.draw(mSelectionHighlight, states);
    target.draw(mText, states);

    if (mShowCursor) 
    {
        target.draw(mCursor, states);
    }
}

void GUI::TextBox::executeBackspace()
{
    if (deleteSelectedText())
    {
        return;
    }
    if (mCursorIndex > 0) 
    {
        updateString(mString.erase(mCursorIndex - 1, 1), mCursorIndex - 1);
    }
}

void GUI::TextBox::executeMoveLeft()
{
    if (hasHighlight())
    {
        assert(!mShowCursor);

        setCursorPosition(std::min(*mSelectionStart, *mSelectionEnd));
        return;
    }

    if (mCursorIndex > 0)    
    {
        setCursorPosition(mCursorIndex - 1);
    }
}
        
void GUI::TextBox::executeMoveRight()
{
    if (hasHighlight())
    {
        assert(!mShowCursor);

        setCursorPosition(std::max(*mSelectionStart, *mSelectionEnd));
        return;
    }

    if (mCursorIndex < mString.size())
    {
        setCursorPosition(mCursorIndex + 1);
    }
}

void GUI::TextBox::setCursorPosition(std::size_t index)
{ // Clear highlights
    mCursorIndex = index;
    mSelectionStart = index;
    mSelectionEnd = index; 

    updateLayout();
    updateHighlight();
    resetCursorBlink();
}

void GUI::TextBox::setSelectionEnd(std::size_t index)
{ // Likely highlights
    mSelectionEnd = index;
    mCursorIndex = index;

    mShowCursor = false;
    updateLayout(); 
    updateHighlight();
}

void GUI::TextBox::updateString(const std::string& newString, std::size_t newCursorPos)
{
    mString = newString;
    mText.setString(mString);

    // Automatically handles layout, highlight clearing, and blinking
    setCursorPosition(newCursorPos); 
}