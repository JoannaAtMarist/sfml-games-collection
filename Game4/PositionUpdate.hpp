#pragma once
#include <memory>
#include "Update.hpp"
#include <SFML/System.hpp>

// Forward declarations to satisfy the assemble() signature
class LevelUpdate;
class PlayerUpdate;

class PositionUpdate : public Update
{
  private:
    sf::Vector2f m_Pos{0.f, 0.f};

  public:
    PositionUpdate() = default;

    void setPosition(float x, float y)
    {
        m_Pos = {x, y};
    }
    sf::Vector2f getPosition() const
    {
        return m_Pos;
    }

    // Decorative flowers don't move
    void update(float) override
    {
    }

    // No assembly needed for PositionUpdate
    void assemble(std::shared_ptr<LevelUpdate>,
                  std::shared_ptr<PlayerUpdate>) override
    {
    }
};
