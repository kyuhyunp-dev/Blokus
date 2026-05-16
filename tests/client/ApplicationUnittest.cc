#include <gtest/gtest.h>
#include "Application.hpp"
#include "Mock/Resource/MockFontHolder.hpp" 


class TestableApplication : public Application
{
public:
    // Inherit the constructor cleanly
    using Application::Application;

    // Elevate the protected methods to public for the test runner
    using Application::updateView;
    using Application::getMainView;
};

#include "Mock/Resource/MockFontHolder.hpp" // Or your real FontHolder setup

class ApplicationTest : public ::testing::Test 
{
protected:
    MockFontHolder mFonts;

    void SetUp() override 
    {
        mFonts.load(Fonts::Sansation, "assets/fonts/Sansation.ttf");
    }
};

TEST_F(ApplicationTest, UpdateViewUltraWide)
{
    // Arrange: Create our testable wrapper instance
    TestableApplication app(mFonts);

    // Act: Simulate resizing the window to an Ultra-wide 21:9 resolution (e.g., 2560x1080)
    // Your virtual target resolution is 16:9 (defined in Config::VirtualRes)
    app.updateView(sf::Vector2u(2560, 1080));

    // Assert: Check the resulting viewport calculations
    sf::FloatRect viewport = app.getMainView().getViewport();

    // Since the window is wider than 16:9, it should add pillarboxes (black bars on the sides)
    // This means the viewport width will shrink below 1.0f, centered on the screen.
    EXPECT_LT(viewport.size.x, 1.0f); 
    EXPECT_EQ(viewport.size.y, 1.0f); // Height should remain fully filled (100%)
    EXPECT_EQ(viewport.position.x, 0.f); // It should be offset from the left edge to center it
}

TEST_F(ApplicationTest, UpdateViewTall)
{
    TestableApplication app(mFonts);

    // Act: Simulate a squarish or tall window (e.g., 1024x1024)
    app.updateView(sf::Vector2u(1024, 1024));

    sf::FloatRect viewport = app.getMainView().getViewport();

    // Since the window is taller than 16:9, it should add letterboxes (black bars on top/bottom)
    EXPECT_EQ(viewport.size.x, 1.0f); // Width should remain fully filled (100%)
    EXPECT_LT(viewport.size.y, 1.0f); // Height should shrink below 1.0f
    EXPECT_EQ(viewport.position.y, 0.f); // It should be offset from the top edge to center it
}