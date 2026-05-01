#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Command/CommandQueue.hpp"
#include "Mock/Query/MockTrayQuery.hpp"
#include "Mock/Query/MockBoardQuery.hpp"
#include "Mock/Nodes/MockBoardNode.hpp"
#include "Mock/Nodes/MockArena.hpp"
#include "Mock/MockReferee.hpp"
#include "shared/PolyominoUtil.hpp"
#include "Utility.hpp"


class PlayerTest : public ::testing::Test {
protected:
    PlayerTest() 
        : mTextures()           // Initialize textures first
        , mTray(mTextures)   // Pass reference to mockTray
        , mBoard()              // Default construct board
        , mPlayer(nullptr)          // Set to null until SetUp
    {
    }

    void SetUp() override
    {
        mWindow.create(sf::VideoMode({800, 600}), "Test Window");
        // RenderWindow can be initialized headlessly in SFML for most logic
        // MockTrayQuery is passed to the player (via a setter or constructor)
        mPlayer = std::make_unique<Player>(mWindow, mReferee);
        mPlayer->setQuery(&mTray, &mBoard);
        
        mTextures.load(Textures::Tiles, "unused_path");
    }

    MockTextureHolder mTextures;
    sf::RenderWindow mWindow;
    MockTrayQuery mTray;
    MockBoardQuery mBoard;
    CommandQueue mCommands;
    std::unique_ptr<Player> mPlayer;
    testing::NiceMock<MockReferee> mReferee;
};


TEST_F(PlayerTest, ValidGrab) {
    // 1. Arrange
    int responseId = 7;
    mTray.setPiece(responseId);

    // Create a fake MouseButtonPressed event
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Left;
    mousePressed.position = {100, 100};  

    // 2. Act
    mPlayer->handleEvent(mousePressed, mCommands);

    // 3. Assert
    EXPECT_FALSE(mCommands.isEmpty());
    EXPECT_EQ(mPlayer->getHeldPieceId(), responseId);

    int size = 0;
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Arena);

        command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    }

    EXPECT_EQ(size, 1);
}

TEST_F(PlayerTest, RightClickGrab) {
    // 1. Arrange
    int responseId = 7; 
    mTray.setPiece(responseId);

    // Create a fake MouseButtonPressed event
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Right;
    mousePressed.position = {100, 100};

    // 2. Act
    mPlayer->handleEvent(mousePressed, mCommands);

    // 3. Assert
    EXPECT_TRUE(mCommands.isEmpty());
    EXPECT_EQ(mPlayer->getHeldPieceId(), std::nullopt);
}

TEST_F(PlayerTest, DoubleGrab) {
    // 1. Arrange: Player is already holding a piece
    int responseId = 7; 
    mTray.setPiece(responseId); 
    
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Left;
    mousePressed.position = {100, 100};    
         
    mPlayer->handleEvent(mousePressed, mCommands); // First click
    EXPECT_FALSE(mCommands.isEmpty());
    EXPECT_EQ(mPlayer->getHeldPieceId(), 7);
    
    int size = 0;
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Arena);

        command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    }

    EXPECT_EQ(size, 1); 

    ON_CALL(mReferee, isValid(responseId, testing::_, testing::_))
    .WillByDefault(testing::Return(false));
    // 2. Act: Click again
    size = 0;
    mPlayer->handleEvent(mousePressed, mCommands);
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    }
    EXPECT_EQ(size, 1);
    EXPECT_EQ(mPlayer->getHeldPieceId(), 7);
    
    // 3. Assert
    responseId = 8;
    mTray.setPiece(responseId);

    size = 0;
    mPlayer->handleEvent(mousePressed, mCommands);
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    } 
    EXPECT_EQ(size, 1);
    EXPECT_EQ(mPlayer->getHeldPieceId(), 7);
} 

TEST_F(PlayerTest, Move) {
    sf::Event moveEvent = sf::Event::MouseMoved{{500, 600}};

    mPlayer->handleEvent(moveEvent, mCommands);
    EXPECT_TRUE(mCommands.isEmpty());

    // Click piece 7
    int responseId = 7; 
    mTray.setPiece(responseId); 
    
    sf::Event::MouseButtonPressed mousePressed;
    mousePressed.button = sf::Mouse::Button::Left;
    mousePressed.position = {100, 100}; 

    mPlayer->handleEvent(mousePressed, mCommands);
    
    int size = 0;
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Arena);

        command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        ++size;
    }
    EXPECT_EQ(size, 1); 
        
    // Mouse Move
    Team color = Team::Red;
    sf::Vector2i grid = {300, 300};

    ON_CALL(mReferee, isValid(responseId, grid, color))
    .WillByDefault(testing::Return(true));

    mPlayer->setTeam(color);
    mBoard.setMinSnappedGrid(grid);
    moveEvent = sf::Event::MouseMoved{{300, 300}};
    mPlayer->handleEvent(moveEvent, mCommands);

    size = 0;
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Board);

        MockBoardNode spyBoard;
        command.action(spyBoard, sf::Time::Zero);

        EXPECT_EQ(spyBoard.lastPieceId, responseId);
        EXPECT_EQ(spyBoard.lastCoord, grid);
        EXPECT_EQ(spyBoard.lastColor, Utility::getShadowColor(color));

        ++size;
    }
    EXPECT_EQ(size, 1);

    color = Team::Blue;
    grid = {400, 400};     

    ON_CALL(mReferee, isValid(responseId, grid, color))
    .WillByDefault(testing::Return(true));

    mPlayer->setTeam(color);
    mBoard.setMinSnappedGrid(grid);
    moveEvent = sf::Event::MouseMoved{{300, 300}};
    mPlayer->handleEvent(moveEvent, mCommands);

    size = 0;
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        EXPECT_EQ(command.category, Category::Board);
        
        MockBoardNode spyBoard;
        command.action(spyBoard, sf::Time::Zero);

        EXPECT_EQ(spyBoard.lastPieceId, responseId);
        EXPECT_EQ(spyBoard.lastCoord, grid);
        EXPECT_EQ(spyBoard.lastColor, Utility::getShadowColor(color));

        ++size;
    }
    EXPECT_EQ(size, 1);
} 

TEST_F(PlayerTest, Transform) {
    Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

    int initialId = 19; 
    mTray.setPiece(initialId);
    
    // Simulate a grab (Left click)
    sf::Event::MouseButtonPressed pressEvent;
    pressEvent.button = sf::Mouse::Button::Left;
    pressEvent.position = {100, 100};
    mPlayer->handleEvent(pressEvent, mCommands);

    // Rotate CW
    sf::Event cwEvent = sf::Event::KeyPressed{sf::Keyboard::Key::R};
    mPlayer->handleEvent(cwEvent, mCommands);

    int cwId = library.clockwiseRotatedIds[initialId];
    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), cwId); 

    // Rotate CCW
    sf::Event ccwEvent = sf::Event::KeyPressed{sf::Keyboard::Key::C};
    mPlayer->handleEvent(ccwEvent, mCommands);

    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), initialId); 

    // Horizontal Reflection
    int hId = library.horizontallyReflectedIds[initialId];
    sf::Event hEvent = sf::Event::KeyPressed{sf::Keyboard::Key::H};
    mPlayer->handleEvent(hEvent, mCommands);

    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), hId); 

    // Vertical Reflection
    int vId = library.clockwiseRotatedIds[cwId];
    sf::Event vEvent = sf::Event::KeyPressed{sf::Keyboard::Key::V};
    mPlayer->handleEvent(vEvent, mCommands);

    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), vId); 
}

TEST_F(PlayerTest, ValidPlacement) {
    int pieceId = 7;
    Team currentTeam = Team::Red;
    sf::Vector2i expectedGrid = {300, 300};

    mPlayer->setTeam(currentTeam);
    mTray.setPiece(pieceId);
    mBoard.setMinSnappedGrid(expectedGrid);

    EXPECT_CALL(mReferee, isValid(pieceId, expectedGrid, currentTeam))
        .WillRepeatedly(testing::Return(true));

    EXPECT_CALL(mReferee, place(pieceId, expectedGrid, currentTeam))
        .Times(1);

    sf::Event::MouseButtonPressed grabClick;
    grabClick.button = sf::Mouse::Button::Left;
    grabClick.position = {100, 100};
    mPlayer->handleEvent(grabClick, mCommands);

    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    while (!mCommands.isEmpty()) {
        mCommands.pop();
    }

    sf::Event::MouseButtonPressed placeClick;
    placeClick.button = sf::Mouse::Button::Left;
    placeClick.position = {400, 400}; 
    mPlayer->handleEvent(placeClick, mCommands);

    EXPECT_FALSE(mPlayer->getHeldPieceId().has_value());

    int size = 0;
    while (!mCommands.isEmpty()) 
    {
        Command command = mCommands.pop(); 
        
        EXPECT_EQ(command.category, Category::Arena);
        ++size;

        MockArena spyArena(mWindow, mTextures, {}, mCommands, currentTeam);

        command.action(spyArena, sf::Time::Zero);

        EXPECT_TRUE(spyArena.placePieceCalled);
        EXPECT_EQ(spyArena.lastPlacedGrid, expectedGrid);
    }

    EXPECT_EQ(size, 1);
}
