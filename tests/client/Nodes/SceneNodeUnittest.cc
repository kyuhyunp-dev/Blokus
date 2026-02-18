#include <gtest/gtest.h>
#include "Nodes/SceneNode.hpp" 
#include "SFML/Graphics/RenderTexture.hpp"


TEST(SceneNodeTest, ParentChildRelationship) {
    // No child test
    SceneNode parent;
    EXPECT_EQ(parent.getChildCount(), 0);
    EXPECT_EQ(parent.getParent(), nullptr);

    auto firstChild = std::make_unique<SceneNode>();
    SceneNode* firstChildPtr = firstChild.get();

    auto secondChild = std::make_unique<SceneNode>();
    SceneNode* secondChildPtr = secondChild.get();

    auto thirdChild = std::make_unique<SceneNode>();
    SceneNode* thirdChildPtr = thirdChild.get();

    // Attach first dhild and test
    parent.attachChild(std::move(firstChild));
    EXPECT_EQ(parent.getChildCount(), 1);
    EXPECT_EQ(firstChildPtr->getParent(), &parent);

    // Attach second child and test
    parent.attachChild(std::move(secondChild));
    EXPECT_EQ(parent.getChildCount(), 2);
    EXPECT_EQ(firstChildPtr->getParent(), &parent);

    // Attach third child to first child and test
    firstChildPtr->attachChild(std::move(thirdChild));
    EXPECT_EQ(firstChildPtr->getChildCount(), 1);
    EXPECT_EQ(thirdChildPtr->getParent(), firstChildPtr);


    // Detach second child with no child
    auto detachedSecond = parent.detachChild(*secondChildPtr);
    EXPECT_EQ(detachedSecond.get(), secondChildPtr);

    EXPECT_EQ(parent.getChildCount(), 1);
    EXPECT_EQ(detachedSecond->getParent(), nullptr);

    // Detach first child with a child
    auto detachedFirst = parent.detachChild(*firstChildPtr);
    EXPECT_EQ(parent.getChildCount(), 0);
    EXPECT_EQ(detachedFirst->getChildCount(), 1); 
}

TEST(SceneNodeTest, WorldTransformCalculation) {
    // Test world position for no parent
    SceneNode parent;
    parent.setPosition({10.f, 10.f});
    sf::Vector2f worldPos = parent.getWorldPosition();
    EXPECT_FLOAT_EQ(worldPos.x, 10.f);
    EXPECT_FLOAT_EQ(worldPos.y, 10.f);

    // Test world position for child
    auto child = std::make_unique<SceneNode>();
    child->setPosition({5.f, 5.f});
    SceneNode* childPtr = child.get();

    parent.attachChild(std::move(child));

    worldPos = childPtr->getWorldPosition();
    EXPECT_FLOAT_EQ(worldPos.x, 15.f);
    EXPECT_FLOAT_EQ(worldPos.y, 15.f);
}

TEST(SceneNodeTest, ReattachChild) {
    SceneNode parentA, parentB;
    auto child = std::make_unique<SceneNode>();
    SceneNode* childPtr = child.get();

    parentA.attachChild(std::move(child));
    parentB.attachChild(parentA.detachChild(*childPtr)); // Standard move

    EXPECT_EQ(childPtr->getParent(), &parentB);
    EXPECT_EQ(parentA.getChildCount(), 0);
}

TEST(SceneNodeTest, RotationTransform) {
    SceneNode parent;
    // Clockwise 90 degrees
    parent.setRotation(sf::degrees(90.f)); 

    auto child = std::make_unique<SceneNode>();
    child->setPosition({10.f, 5.f}); 
    SceneNode* childPtr = child.get();

    parent.attachChild(std::move(child));

    sf::Vector2f worldPos = childPtr->getWorldPosition();
    // After 90 deg rotation, (10, 5) becomes (-5, 10)
    EXPECT_NEAR(worldPos.x, -5.f, 0.01f); 
    EXPECT_NEAR(worldPos.y, 10.f, 0.01f);
}
