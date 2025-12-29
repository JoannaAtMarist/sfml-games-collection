#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Update.hpp"

class LevelUpdate;
class PlayerUpdate;

// -------------------------------------------------------------
// PlatformUpdate
//  - Stores the platform's rectangle in the world
//  - (Later) can handle collision with the player.
// -------------------------------------------------------------
class PlatformUpdate : public Update
{
  private:
    sf::FloatRect m_Position{};

    // For collision with player
    sf::FloatRect *m_PlayerPosition = nullptr;
    bool *m_PlayerIsGrounded = nullptr;

    // full Player pointer so we can grant powers
    PlayerUpdate *m_Player = nullptr;

    // flight settings
    bool m_GrantsFlight = false;
    float m_FlightSeconds = 3.f;

    // Lethal & disappearing behavior
    int m_DeadlyLevel = 0;
    bool m_Deadly = false;
    bool m_Disappearing = false;
    float m_DisappearDelay = 0.f; // seconds before vanish
    float m_TimeOnPlatform = 0.f; // how long the player has stood on it
    bool m_PlayerOnThis = false;  // set each frame when landed
    bool m_Active = true;         // false = no collision / no draw

    LevelUpdate *m_LevelUpdate = nullptr;

  public:
    PlatformUpdate() = default;

    // Access to the underlying rectangle (for legacy uses)
    sf::FloatRect *getPositionPointer();

    // Simple position/size helpers for graphics
    void setPosition(float x, float y);
    void setSize(float w, float h);

    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;

    // From Update : Component
    void update(float fps) override;
    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    void setGrantsFlight(bool grants, float seconds = 3.f);
    void setDeadly(bool deadly, int activeLevel = 0);
    void setDisappearing(bool disappearing, float delaySeconds = 2.f);
    bool isActive() const;

    // Reset logic for when the platform is recycled or level restarts
    void reset();
};
