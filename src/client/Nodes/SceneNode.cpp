#include "Nodes/SceneNode.hpp" 
#include "Command/Command.hpp"

#include <algorithm>
#include <cassert>


SceneNode::SceneNode()
    : mChildren()
    , mParent(nullptr){}

void SceneNode::attachChild(Ptr child)
{
    assert(child->mParent == nullptr);
    child->mParent = this;
    mChildren.emplace_back(std::move(child));
}

SceneNode::Ptr SceneNode::detachChild(const SceneNode& node)
{
    auto it = std::find_if(mChildren.begin(), mChildren.end(),
        [&](const Ptr& child){
            return child.get() == &node;
        });
    assert(it != mChildren.end());

    auto ret = std::move(*it);
    ret->mParent = nullptr;
    mChildren.erase(it);

    return ret;
}

void SceneNode::update(sf::Time dt)
{
    updateCurrent(dt);
    updateChildren(dt);
}

void SceneNode::updateCurrent(sf::Time)
{ // Handled by derived functions
}

void SceneNode::updateChildren(sf::Time dt)
{
    for(auto& child : mChildren)
    {
        child->update(dt);
    }
}

void SceneNode::draw(sf::RenderTarget& target, 
    sf::RenderStates states) const
{
    // Apply transformation of current node
    states.transform *= getTransform();

    drawCurrent(target, states);
    drawChildren(target, states);
}

void SceneNode::drawCurrent(sf::RenderTarget& target,
    sf::RenderStates states) const
{ // Handled by derived functions
}

void SceneNode::drawChildren(sf::RenderTarget& target,
    sf::RenderStates states) const
{
    for (const auto& child : mChildren)
    {
        child->draw(target, states);
    }
}

sf::Vector2f SceneNode::getWorldPosition() const
{
    return getWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::getWorldTransform() const
{
    sf::Transform transform = sf::Transform::Identity;

    for (const SceneNode* node = this; node != nullptr; node = node->mParent)
    {
        transform = node->getTransform() * transform;
    }

    return transform;
}

size_t SceneNode::getChildCount() const 
{ 
    return mChildren.size(); 
}

std::vector<SceneNode*> SceneNode::getChildren() const 
{
    std::vector<SceneNode*> children;
    for (const auto& child : mChildren) {
        children.push_back(child.get());
    }
    return children;
}
        
SceneNode* SceneNode::getParent() const 
{ 
    return mParent; 
}

void SceneNode::clearChildren()
{
    mChildren.clear();
}

bool SceneNode::contains(sf::Vector2f worldPoint) const 
{
    for (const auto& child : mChildren) 
    {
        if (child->contains(worldPoint)) 
            return true;
    }

    return false;
}

void SceneNode::onCommand(const Command& command, sf::Time dt)
{
    // 1. Check if this specific node belongs to the targeted category
    // Using bitwise AND (&) allows one node to belong to multiple categories
    if (command.category & getCategory())
    {
        command.action(*this, dt);
    }

    // 2. Recursively pass the command to all children
    for (auto& child : mChildren)
    {
        child->onCommand(command, dt);
    }
}

unsigned int SceneNode::getCategory() const
{
    return Category::Scene;
}
