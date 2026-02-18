#include "Nodes/SceneNode.hpp" 
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
        
SceneNode* SceneNode::getParent() const 
{ 
    return mParent; 
}