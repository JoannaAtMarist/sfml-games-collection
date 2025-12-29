#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Component.hpp"

class LevelUpdate;
class PlayerUpdate;

// -------------------------------------------------------------
// Update (base class)
//  - Any component that updates game logic inherits from this.
// -------------------------------------------------------------
class Update : public Component
{
  private:
  
  public:
    Update(); // sets m_IsUpdate = true

    virtual void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                          std::shared_ptr<PlayerUpdate> playerUpdate) = 0;

    virtual void update(float timeSinceLastUpdate) = 0;
};