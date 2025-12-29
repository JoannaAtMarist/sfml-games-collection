#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Update.hpp"
#include "PlayerUpdate.hpp"

class LevelUpdate;

class MushroomPerilUpdate : public Update
{
  private:
    sf::Vector2f m_Position{0.f, 0.f};          // where we draw the sprite
    sf::FloatRect m_Area{0.f, 0.f, 64.f, 32.f}; // hurtbox near the base

    sf::FloatRect *m_PlayerRect = nullptr;
    PlayerUpdate *m_Player = nullptr;

    LevelUpdate *m_LevelUpdate = nullptr; // We need to store this now!
    int m_ActiveLevel = 0;                // 0 = always active
    int m_Damage = 1;                     // Default damage

  public:
    MushroomPerilUpdate();

    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    void update(float dt) override;

    void setPosition(float x, float y);
    void setAreaSize(float width, float height);

    // Getters
    const sf::Vector2f &getPosition() const
    {
        return m_Position;
    }
    const sf::FloatRect &getArea() const
    {
        return m_Area;
    }

    // Setters
    void setDamage(int dmg)
    {
        m_Damage = dmg;
    }
    void setActiveLevel(int level)
    {
        m_ActiveLevel = level;
    }
};
