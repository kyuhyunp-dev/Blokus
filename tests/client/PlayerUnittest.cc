#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Command/CommandQueue.hpp"
#include "Mock/Query/MockTrayQuery.hpp"
#include "Mock/Query/MockBoardQuery.hpp"
#include "Mock/Nodes/MockBoardNode.hpp"
#include "shared/PolyominoUtil.hpp"
#include "Config.hpp"


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

        command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
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

        command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    }

    EXPECT_EQ(size, 1);

    // 2. Act: Click again
    size = 0;
    player->handleEvent(mousePressed, commands);
    while (!commands.isEmpty()) 
    {
        Command command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    }
    EXPECT_EQ(size, 1);
    EXPECT_EQ(player->getHeldPieceId(), 7);
    
    // 3. Assert
    responseId = 8;
    mockTray.setPiece(responseId);

    size = 0;
    player->handleEvent(mousePressed, commands);
    while (!commands.isEmpty()) 
    {
        Command command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    } 
    EXPECT_EQ(size, 1);
    EXPECT_EQ(player->getHeldPieceId(), 7);
} 

TEST_F(PlayerTest, Move) {
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
    
    int size = 0;
    while (!commands.isEmpty()) 
    {
        Command command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Arena);

        command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    }
    EXPECT_EQ(size, 1); 
        
    // Mouse Move
    Team color = Team::Red;
    sf::Vector2i grid = {300, 300};

    player->setTeam(color);
    mockBoard.setMinSnappedGrid(grid);
    moveEvent = sf::Event::MouseMoved{{300, 300}};
    player->handleEvent(moveEvent, commands);

    size = 0;
    while (!commands.isEmpty()) 
    {
        Command command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Board);

        MockBoardNode spyBoard(mockTextures);
        command.action(spyBoard, sf::Time::Zero);

        EXPECT_EQ(spyBoard.lastPieceId, responseId);
        EXPECT_EQ(spyBoard.lastCoord, grid);
        EXPECT_EQ(spyBoard.lastColor, Config::getShadowColor(color));

        ++size;
    }
    EXPECT_EQ(size, 1);

    color = Team::Blue;
    grid = {400, 400};

    player->setTeam(color);
    mockBoard.setMinSnappedGrid(grid);
    moveEvent = sf::Event::MouseMoved{{300, 300}};
    player->handleEvent(moveEvent, commands);

    size = 0;
    while (!commands.isEmpty()) 
    {
        Command command = commands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        
        MockBoardNode spyBoard(mockTextures);
        command.action(spyBoard, sf::Time::Zero);

        EXPECT_EQ(spyBoard.lastPieceId, responseId);
        EXPECT_EQ(spyBoard.lastCoord, grid);
        EXPECT_EQ(spyBoard.lastColor, Config::getShadowColor(color));

        ++size;
    }
    EXPECT_EQ(size, 1);
} 

TEST_F(PlayerTest, Transform) {
    Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

    int initialId = 19; 
    mockTray.setPiece(initialId);
    
    // Simulate a grab (Left click)
    sf::Event::MouseButtonPressed pressEvent;
    pressEvent.button = sf::Mouse::Button::Left;
    pressEvent.position = {100, 100};
    player->handleEvent(pressEvent, commands);

    // Rotate CW
    sf::Event cwEvent = sf::Event::KeyPressed{sf::Keyboard::Key::R};
    player->handleEvent(cwEvent, commands);

    int cwId = library.clockwiseRotatedIds[initialId];
    ASSERT_TRUE(player->getHeldPieceId().has_value());
    EXPECT_EQ(player->getHeldPieceId().value(), cwId); 

    sf::Event ccwEvent = sf::Event::KeyPressed{sf::Keyboard::Key::C};
    player->handleEvent(ccwEvent, commands);

    ASSERT_TRUE(player->getHeldPieceId().has_value());
    EXPECT_EQ(player->getHeldPieceId().value(), initialId); 

    // Horizontal Reflection
    int hId = library.horizontallyReflectedIds[initialId];
    sf::Event hEvent = sf::Event::KeyPressed{sf::Keyboard::Key::H};
    player->handleEvent(hEvent, commands);

    ASSERT_TRUE(player->getHeldPieceId().has_value());
    EXPECT_EQ(player->getHeldPieceId().value(), hId); 

    // Vertical Reflection
    int vId = library.clockwiseRotatedIds[cwId];
    sf::Event vEvent = sf::Event::KeyPressed{sf::Keyboard::Key::V};
    player->handleEvent(vEvent, commands);

    ASSERT_TRUE(player->getHeldPieceId().has_value());
    EXPECT_EQ(player->getHeldPieceId().value(), vId); 
}
