#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "GameObject.hpp"
#include "InputDispatcher.hpp"

class InputDispatcher;

// -------------------------------------------------------------
// Factory
//  - Responsible for building all game objects and assembling
//    their components (graphics, update, input, etc).
// -------------------------------------------------------------
class Factory
{
  private:
    sf::RenderWindow *m_Window;

  public:
    Factory(sf::RenderWindow *window);

    // For Step 1: only build a single test platform
    void loadLevel(std::vector<GameObject *> &gameObjects,
                   InputDispatcher &inputDispatcher);
};
