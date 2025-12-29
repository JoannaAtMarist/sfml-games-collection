#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Update.hpp"

class PlayerUpdate;
class LevelUpdate;

class RoadUpdate : public Update
{
  private:
    sf::FloatRect m_Position;
    sf::FloatRect *m_PlayerPos = nullptr;
    bool *m_PlayerGrounded = nullptr;

  public:
    RoadUpdate()
    {
        m_IsUpdate = true;
    }

    void setPosition(float x, float y)
    {
        m_Position.left = x;
        m_Position.top = y;
    }
    void setSize(float w, float h)
    {
        m_Position.width = w;
        m_Position.height = h;
    }

    sf::FloatRect *getPositionPointer()
    {
        return &m_Position;
    }

    // FROM Update
    void assemble(std::shared_ptr<LevelUpdate> level,
                  std::shared_ptr<PlayerUpdate> player) override;

    void update(float dt) override;
};
