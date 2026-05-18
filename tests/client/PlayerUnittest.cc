#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Command/CommandQueue.hpp"
#include "Mock/Query/MockTrayQuery.hpp"
#include "Mock/Query/MockBoardQuery.hpp"
#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Nodes/MockBoardNode.hpp"
#include "Mock/Nodes/MockArena.hpp"
#include "shared/Mock/MockReferee.hpp"
#include "shared/PolyominoUtil.hpp"
#include "Utility.hpp"


class PlayerTest : public ::testing::Test {
protected:
    PlayerTest() 
        : mTextures()           
        , mTray()   
        , mBoard()              
        , mPlayer(nullptr)     
        , mLibrary(Blokus::PolyominoGenerator::getData())
    {
    }

    void SetUp() override
    {
        mWindow.create(sf::VideoMode({800, 600}), "Test Window");
        // RenderWindow can be initialized headlessly in SFML for most logic
        // MockTrayQuery is passed to the player (via a setter or constructor)
        mPlayer = std::make_unique<Player>(mWindow, mReferee);
        mPlayer->setTeam(Team::Blue);
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
    Blokus::PolyominoDefinition mLibrary;
};

TEST_F(PlayerTest, ValidPlacement) {
    int pieceId = 7;
    Team currentTeam = Team::Red;
    sf::Vector2i failGrid = {300, 300};
    sf::Color invalidShadow = getShadowColor(Team::None);
    
    testing::NiceMock<MockArena> mockArena;
    ON_CALL(mockArena, getCategory())
        .WillByDefault(testing::Return(Category::Arena)); 

    testing::NiceMock<MockBoardNode> mockBoard;
    
    mPlayer->setTeam(currentTeam);
    PieceNode dummyPiece(pieceId, currentTeam, mTextures);

    // Grab Piece -> TrayQuery::getPieceAt, BoardNode::getMinSnappedGrid 
    EXPECT_CALL(mTray, getPieceAt(testing::_))
        .WillOnce(testing::Return(&dummyPiece)); 

    EXPECT_CALL(mBoard, getMinSnappedGrid(testing::_, testing::_))
        .WillOnce(testing::Return(failGrid));

    EXPECT_CALL(mReferee, isValid(pieceId, failGrid, currentTeam))
        .WillOnce(testing::Return(false));

    sf::Event::MouseButtonPressed grabClick;
    grabClick.button = sf::Mouse::Button::Left;
    grabClick.position = failGrid;
    mPlayer->handleEvent(grabClick, mCommands);

    EXPECT_EQ(mPlayer->getHeldPieceId(), pieceId);

    // Arena::grabPiece, Board::updateShadow
    bool grabbedPiece = false;
    bool updatedShadow = false;
    while (!mCommands.isEmpty()) {
        Command command = mCommands.pop();

        if (command.category == Category::Arena) {
            EXPECT_CALL(mockArena, grabPiece(pieceId, testing::_))
                .Times(1);

            command.action(mockArena, sf::Time::Zero);
            grabbedPiece = true;
        }
        else if (command.category == Category::Board) {
            EXPECT_CALL(mockBoard, 
                updateShadow(pieceId, testing::_, invalidShadow))
                .Times(1);

            command.action(mockBoard, sf::Time::Zero);
            updatedShadow = true;
        }
        else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(grabbedPiece);
    EXPECT_TRUE(updatedShadow);

    // Move Piece -> BoardNode::getMinSnappedGrid, Referee::isValid
    sf::Vector2i successGrid = {500, 500};
    sf::Color teamShadow = getShadowColor(currentTeam);

    EXPECT_CALL(mBoard, getMinSnappedGrid(testing::_, testing::_))
        .WillOnce(testing::Return(successGrid));

    // Simulate Referee validating the move as TRUE
    EXPECT_CALL(mReferee, isValid(pieceId, successGrid, currentTeam))
        .WillOnce(testing::Return(true));

    sf::Event::MouseMoved moveEvent;
    moveEvent.position = successGrid;
    
    mPlayer->handleEvent(moveEvent, mCommands);

    // Board::updateShadow
    updatedShadow = false;
    while (!mCommands.isEmpty()) {
        Command command = mCommands.pop();

        if (command.category == Category::Board) {
            EXPECT_CALL(mockBoard, 
                updateShadow(pieceId, testing::_, teamShadow))
                .Times(1);

            command.action(mockBoard, sf::Time::Zero);
            updatedShadow = true;
        }
        else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(updatedShadow);

    // Place Piece -> BoardNode::getMinSnappedGrid, Referee::isValid, Referee::place
    EXPECT_CALL(mBoard, getMinSnappedGrid(testing::_, testing::_))
        .WillOnce(testing::Return(successGrid));

    EXPECT_CALL(mReferee, isValid(pieceId, successGrid, currentTeam))
        .WillOnce(testing::Return(true));

    EXPECT_CALL(mReferee, place(pieceId, successGrid, currentTeam))
        .Times(1);

    sf::Event::MouseButtonPressed placeClick;
    placeClick.button = sf::Mouse::Button::Left;
    placeClick.position = successGrid;

    mPlayer->handleEvent(placeClick, mCommands);

    // Arena::placePiece, Board::clearShadow
    bool placedPiece = false;
    bool clearedShadow = false;
    while (!mCommands.isEmpty()) {
        Command command = mCommands.pop();

        if (command.category == Category::Arena) {
            EXPECT_CALL(mockArena, 
                placePiece(successGrid))
                .Times(1);

            command.action(mockArena, sf::Time::Zero);
            placedPiece = true;
        }
        else if (command.category == Category::Board) {
            EXPECT_CALL(mockBoard, 
                clearShadow())
                .Times(1);

            command.action(mockBoard, sf::Time::Zero);
            clearedShadow = true;
        }
        else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(placedPiece);
    EXPECT_TRUE(clearedShadow);
}

TEST_F(PlayerTest, ReturnTransformedPiece) {
    int pieceId = 7;
    Team currentTeam = Team::Red;
    sf::Vector2i failGrid = {100, 100};
    sf::Color invalidShadow = getShadowColor(Team::None);

    testing::NiceMock<MockArena> mockArena;
    ON_CALL(mockArena, getCategory())
        .WillByDefault(testing::Return(Category::Arena)); 

    testing::NiceMock<MockBoardNode> mockBoard;


    mPlayer->setTeam(currentTeam);
    PieceNode dummyNode(pieceId, currentTeam, mTextures);

    // Grab Piece -> TrayQuery::getPieceAt, BoardNode::getMinSnappedGrid 
    EXPECT_CALL(mTray, getPieceAt(testing::_))
        .WillOnce(testing::Return(&dummyNode)); 

    EXPECT_CALL(mBoard, getMinSnappedGrid(testing::_, testing::_))
        .WillOnce(testing::Return(failGrid));

    EXPECT_CALL(mReferee, isValid(pieceId, failGrid, currentTeam))
        .WillOnce(testing::Return(false));

    sf::Event::MouseButtonPressed grabClick;
    grabClick.button = sf::Mouse::Button::Left;
    grabClick.position = failGrid;
    mPlayer->handleEvent(grabClick, mCommands);

    // Commands for grabPiece already tested
    while (!mCommands.isEmpty()) {
        mCommands.pop();
    }

    // Transform
    int rotatedId = mLibrary.clockwiseRotatedIds[pieceId];
    EXPECT_CALL(mBoard, getMinSnappedGrid(testing::_, testing::_))
        .WillOnce(testing::Return(failGrid));

    EXPECT_CALL(mReferee, isValid(rotatedId, failGrid, currentTeam))
        .WillOnce(testing::Return(false)); 

    sf::Event cwEvent = sf::Event::KeyPressed{sf::Keyboard::Key::R};
    mPlayer->handleEvent(cwEvent, mCommands);

    EXPECT_EQ(mPlayer->getHeldPieceId(), rotatedId);

    bool updatedShadow = false;
    while (!mCommands.isEmpty()) {
        Command command = mCommands.pop();

        if (command.category == Category::Board) {
            EXPECT_CALL(mockBoard, 
                updateShadow(rotatedId, testing::_, invalidShadow))
                .Times(1);

            command.action(mockBoard, sf::Time::Zero);
            updatedShadow = true;
        }
        else {
            EXPECT_TRUE(false);
        }
    }
    EXPECT_TRUE(updatedShadow);

    // Invalid Placement -> BoardNode::getMinSnappedGrid, Referee::isValid
    sf::Vector2i invalidGrid = {300, 300};

    EXPECT_CALL(mBoard, getMinSnappedGrid(testing::_, testing::_))
        .WillOnce(testing::Return(invalidGrid));

    EXPECT_CALL(mReferee, isValid(rotatedId, invalidGrid, currentTeam))
        .WillOnce(testing::Return(false));

    sf::Event::MouseButtonPressed placeClick;
    placeClick.button = sf::Mouse::Button::Left;
    placeClick.position = invalidGrid;

    mPlayer->handleEvent(placeClick, mCommands);

    EXPECT_FALSE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(dummyNode.getId(), pieceId); 

    // Arena::returnPiece, BoardNode::clearShadow
    bool returnedPiece = false;
    bool clearedShadow = false;

    while (!mCommands.isEmpty()) {
        Command command = mCommands.pop();

        if (command.category == Category::Arena) {    
            EXPECT_CALL(mockArena, returnPiece())
                .Times(1);

            command.action(mockArena, sf::Time::Zero);
            returnedPiece = true;
        }
        else if (command.category == Category::Board) {
            EXPECT_CALL(mockBoard, clearShadow())
                .Times(1);

            command.action(mockBoard, sf::Time::Zero);
            clearedShadow = true;
        }
        else {
            // If any other command category slipped in, fail the test
            EXPECT_TRUE(false); 
        }
    }
    EXPECT_TRUE(returnedPiece);
    EXPECT_TRUE(clearedShadow);
}

TEST_F(PlayerTest, IgnoreInvalidInputs) {
    mPlayer->setTeam(Team::Blue);

    // Right Click
    sf::Event::MouseButtonPressed rightClick;
    rightClick.button = sf::Mouse::Button::Right;
    rightClick.position = {100, 100};

    mPlayer->handleEvent(rightClick, mCommands);
    EXPECT_TRUE(mCommands.isEmpty()); 
    EXPECT_FALSE(mPlayer->getHeldPieceId().has_value()); 

    // Left Click on an empty tray 
    EXPECT_CALL(mTray, getPieceAt(testing::_))
        .WillOnce(testing::Return(nullptr));

    sf::Event::MouseButtonPressed leftClickEmpty;
    leftClickEmpty.button = sf::Mouse::Button::Left;
    leftClickEmpty.position = {200, 200};

    mPlayer->handleEvent(leftClickEmpty, mCommands);
    
    EXPECT_TRUE(mCommands.isEmpty()); 
    EXPECT_FALSE(mPlayer->getHeldPieceId().has_value());
}

TEST_F(PlayerTest, Transform) 
{
    // Grab Piece
    int initialId = 19;
    Team currentTeam = Team::Red;
    sf::Vector2i failGrid = {100, 100};

    testing::NiceMock<MockArena> mockArena;
    ON_CALL(mockArena, getCategory())
        .WillByDefault(testing::Return(Category::Arena)); 

    testing::NiceMock<MockBoardNode> mockBoard;

    mPlayer->setTeam(currentTeam);
    PieceNode dummyPiece(initialId, currentTeam, mTextures);

    // Grab Piece -> TrayQuery::getPieceAt, BoardNode::getMinSnappedGrid 
    EXPECT_CALL(mTray, getPieceAt(testing::_))
        .WillOnce(testing::Return(&dummyPiece)); 

    EXPECT_CALL(mBoard, getMinSnappedGrid(testing::_, testing::_))
        .WillRepeatedly(testing::Return(failGrid));

    EXPECT_CALL(mReferee, isValid(testing::_, failGrid, currentTeam))
        .WillRepeatedly(testing::Return(false));

    sf::Event::MouseButtonPressed grabClick;
    grabClick.button = sf::Mouse::Button::Left;
    grabClick.position = failGrid;
    mPlayer->handleEvent(grabClick, mCommands);

    // Commands for grabPiece already tested
    while (!mCommands.isEmpty()) {
        mCommands.pop();
    }

    // Rotate CW
    sf::Event cwEvent = sf::Event::KeyPressed{sf::Keyboard::Key::R};
    mPlayer->handleEvent(cwEvent, mCommands);

    int cwId = mLibrary.clockwiseRotatedIds[initialId];
    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), cwId); 

    // Rotate CCW
    sf::Event ccwEvent = sf::Event::KeyPressed{sf::Keyboard::Key::C};
    mPlayer->handleEvent(ccwEvent, mCommands);

    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), initialId); 

    // Horizontal Reflection
    int hId = mLibrary.horizontallyReflectedIds[initialId];
    sf::Event hEvent = sf::Event::KeyPressed{sf::Keyboard::Key::H};
    mPlayer->handleEvent(hEvent, mCommands);

    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), hId); 

    // Vertical Reflection
    int vId = mLibrary.clockwiseRotatedIds[cwId];
    sf::Event vEvent = sf::Event::KeyPressed{sf::Keyboard::Key::V};
    mPlayer->handleEvent(vEvent, mCommands);

    ASSERT_TRUE(mPlayer->getHeldPieceId().has_value());
    EXPECT_EQ(mPlayer->getHeldPieceId().value(), vId); 
}