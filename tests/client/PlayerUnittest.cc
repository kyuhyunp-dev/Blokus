#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Command/CommandQueue.hpp"
#include "Mock/MockTrayQuery.hpp"


class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        window.create(sf::VideoMode({800, 600}), "Test Window");
        // RenderWindow can be initialized headlessly in SFML for most logic
        // MockTrayQuery is passed to the player (via a setter or constructor)
        player = std::make_unique<Player>(window);
        player->setQuery(&mockTray);
    }

    sf::RenderWindow window;
    MockTrayQuery mockTray;
    CommandQueue commands;
    std::unique_ptr<Player> player;
};

TEST_F(PlayerTest, ValidGrab) {
    // 1. Arrange
    mockTray.responseId = 7; // Pretend Piece #7 is at the click location
    
    // Create a fake MouseButtonPressed event
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Left;
    mousePressed.position = {100, 100};  

    // 2. Act
    player->handleEvent(mousePressed, commands);

    // 3. Assert
    EXPECT_FALSE(commands.isEmpty());
    EXPECT_EQ(player->getHeldPieceId(), mockTray.responseId);

    int size = 0;
    while (!commands.isEmpty()) 
    {
        Command command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Arena);
        ++size;
    }

    EXPECT_EQ(size, 1);
}

TEST_F(PlayerTest, RightClickGrab) {
    // 1. Arrange
    mockTray.responseId = 7; // Pretend Piece #7 is at the click location
    
    // Create a fake MouseButtonPressed event
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Right;
    mousePressed.position = {100, 100};

    // 2. Act
    player->handleEvent(mousePressed, commands);

    // 3. Assert
    EXPECT_TRUE(commands.isEmpty());
    EXPECT_EQ(player->getHeldPieceId(), std::nullopt);
}

TEST_F(PlayerTest, DoubleGrab) {
    // 1. Arrange: Player is already holding a piece
    mockTray.responseId = 7;   
    
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Left;
    mousePressed.position = {100, 100};    

    player->handleEvent(mousePressed, commands); // First click
    EXPECT_FALSE(commands.isEmpty());
    EXPECT_EQ(player->getHeldPieceId(), 7);
    
    int size = 0;
    while (!commands.isEmpty()) 
    {
        Command command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Arena);
        ++size;
    }

    EXPECT_EQ(size, 1);

    // 2. Act: Click again
    player->handleEvent(mousePressed, commands);
    EXPECT_TRUE(commands.isEmpty());
    EXPECT_EQ(player->getHeldPieceId(), 7);
    
    // 3. Assert
    mockTray.responseId = 8;
    player->handleEvent(mousePressed, commands);
    EXPECT_TRUE(commands.isEmpty());
    EXPECT_EQ(player->getHeldPieceId(), 7);
} 