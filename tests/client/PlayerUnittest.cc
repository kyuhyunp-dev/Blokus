#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Command/CommandQueue.hpp"
#include "Mock/Query/MockTrayQuery.hpp"
#include "Mock/Query/MockBoardQuery.hpp"


class PlayerTest : public ::testing::Test {
protected:
    PlayerTest() 
        : mockTextures()           // Initialize textures first
        , mockTray(mockTextures)   // Pass reference to mockTray
        , mockBoard()              // Default construct board
        , player(nullptr)          // Set to null until SetUp
    {
    }

    void SetUp() override
    {
        window.create(sf::VideoMode({800, 600}), "Test Window");
        // RenderWindow can be initialized headlessly in SFML for most logic
        // MockTrayQuery is passed to the player (via a setter or constructor)
        player = std::make_unique<Player>(window);
        player->setQuery(&mockTray, &mockBoard);
        
        mockTextures.load(Textures::Tiles, "unused_path");
    }

    MockTextureHolder mockTextures;
    sf::RenderWindow window;
    MockTrayQuery mockTray;
    MockBoardQuery mockBoard;
    CommandQueue commands;
    std::unique_ptr<Player> player;
};

TEST_F(PlayerTest, ValidGrab) {
    // 1. Arrange
    int responseId = 7;
    mockTray.setPiece(responseId);

    // Create a fake MouseButtonPressed event
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Left;
    mousePressed.position = {100, 100};  

    // 2. Act
    player->handleEvent(mousePressed, commands);

    // 3. Assert
    EXPECT_FALSE(commands.isEmpty());
    EXPECT_EQ(player->getHeldPieceId(), responseId);

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
    int responseId = 7; 
    mockTray.setPiece(responseId);

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
    int responseId = 7; 
    mockTray.setPiece(responseId); 
    
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
    responseId = 8;
    mockTray.setPiece(responseId);

    player->handleEvent(mousePressed, commands);
    EXPECT_TRUE(commands.isEmpty());
    EXPECT_EQ(player->getHeldPieceId(), 7);
} 

TEST_F(PlayerTest, Move) {
    sf::Vector2f targetPos(500.f, 600.f);
    sf::Event moveEvent = sf::Event::MouseMoved{{500, 600}};

    player->handleEvent(moveEvent, commands);
    EXPECT_TRUE(commands.isEmpty());

    // Click piece 7
    int responseId = 7; 
    mockTray.setPiece(responseId); 
    
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Left;
    mousePressed.position = {100, 100}; 

    player->handleEvent(mousePressed, commands);
    
    EXPECT_FALSE(commands.isEmpty());
    Command cmd = commands.pop();
        
    // Mouse Move
    player->handleEvent(moveEvent, commands);

    EXPECT_FALSE(commands.isEmpty());
    cmd = commands.pop();
    
    EXPECT_EQ(cmd.category, Category::ActivePiece);
}