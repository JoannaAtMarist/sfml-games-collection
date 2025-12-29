#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Update.hpp"
#include "PlayerUpdate.hpp"

class LevelUpdate;

class BeeProjectileUpdate : public Update
{
  private:
    sf::FloatRect m_Bounds{0.f, 0.f, 32.f, 32.f}; // size ~ one bee frame
    sf::Vector2f m_Velocity{150.f, 0.f};          // weak, fairly slow

    sf::FloatRect *m_PlayerRect = nullptr;
    PlayerUpdate *m_Player = nullptr;

    bool m_Active = true;

    // Meaner Bees:
    int m_Damage = 1;
    bool m_ZigZag = false;
    float m_ZigAmplitude = 0.f;
    float m_ZigSpeed = 0.f;
    float m_Time = 0.f; // for zigzag timing

    // remember spawn & respawn timing
    sf::Vector2f m_StartPos;    // where this bee originally spawns
    float m_RespawnDelay = 4.f; // seconds to wait before next wave
    float m_InactiveTime = 0.f; // how long we've been "off"

    int m_ActiveLevel = 0; // 0 = always active
    LevelUpdate *m_LevelUpdate = nullptr;

  public:
    BeeProjectileUpdate();

    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    void update(float dt) override;

    void setPosition(float x, float y);
    void setVelocity(float vx, float vy);
    void setDamage(int dmg);
    void enableZigZag(float amplitude, float speed);

    // Accessors
    const sf::FloatRect &getBounds() const
    {
        return m_Bounds;
    }

    // Set the level where this bee is active (0 = always active)
    void setActiveLevel(int level)
    {
        m_ActiveLevel = level;
    }
};
