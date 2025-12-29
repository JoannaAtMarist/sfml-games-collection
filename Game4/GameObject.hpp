#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Component.hpp"

// -------------------------------------------------------------
// GameObject
//  - Container for Components (Graphics + Update).
//  - Routes update() and draw() calls to the correct components.
// -------------------------------------------------------------
class GameObject
{
  private:
    std::vector<std::shared_ptr<Component>> m_Components;

  public:
    void addComponent(std::shared_ptr<Component> newComponent);
    void update(float elapsedTime);
    void draw(sf::RenderWindow &window);
};