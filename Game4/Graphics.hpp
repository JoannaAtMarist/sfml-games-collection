#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Component.hpp"

class Update;

// ------------------------------------------------------------
// Graphics (Base Class)
//
// Every graphics component in the game inherits from this.
// It owns a sprite, and the draw() method will be overridden.
// ------------------------------------------------------------
class Graphics : public Component
{
  public:
    Graphics();
    
    virtual ~Graphics() = default;

    // Graphics components must implement this if they need access to Update
    virtual void assemble(std::shared_ptr<Update> genericUpdate)
    {
    }

    // Draw the sprite
    virtual void draw(sf::RenderWindow &window) = 0;

  protected:
    sf::Sprite m_Sprite; // The visual object all graphics will draw
};