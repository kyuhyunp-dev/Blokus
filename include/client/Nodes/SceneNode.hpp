#ifndef SCENENODE_HPP
#define SCENENODE_HPP

#include "SFML/System/Time.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/Drawable.hpp"

#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderTarget.hpp"

#include <vector>
#include <memory>

class Command;

class SceneNode : public sf::Transformable, public  sf::Drawable
{
    public:
        typedef std::unique_ptr<SceneNode> Ptr;

    public:
        SceneNode();

        void attachChild(Ptr child);

        Ptr detachChild(const SceneNode& node);

        void update(sf::Time dt);

        sf::Vector2f getWorldPosition() const;

        sf::Transform getWorldTransform() const;

        // Helper for your Unit Tests
        size_t getChildCount() const;

        std::vector<SceneNode*> getChildren() const;
        
        SceneNode* getParent() const; 

        void clearChildren();

        virtual bool contains(sf::Vector2f worldPoint) const;
        
        void onCommand(const Command& command, sf::Time dt);

        virtual unsigned int getCategory() const;

    private:
        virtual void updateCurrent(sf::Time dt);
        
        void updateChildren(sf::Time dt);

        void draw(sf::RenderTarget& target, 
            sf::RenderStates states) const;
        
        virtual void drawCurrent(sf::RenderTarget& target,
            sf::RenderStates states) const;
        
        void drawChildren(sf::RenderTarget& target,
            sf::RenderStates states) const;

    private:
        std::vector<Ptr> mChildren;
        SceneNode* mParent;

};


#endif