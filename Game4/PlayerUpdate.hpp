#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "InputReceiver.hpp"
#include "Update.hpp"

// -------------------------------------------------------------
// PlayerUpdate
//  - Handles player movement input, gravity, jumping, boosting,
//    and grounded state.
// -------------------------------------------------------------
class PlayerUpdate : public Update
{
  private:
    const float SPRITE_W = 16.f;
    const float SPRITE_H = 16.f;
    const float SPRITE_SCALE = 3.f;

    const float PLAYER_WIDTH = SPRITE_W * SPRITE_SCALE;  // 48
    const float PLAYER_HEIGHT = SPRITE_H * SPRITE_SCALE; // 48

    sf::FloatRect m_Position;

    bool *m_IsPaused = nullptr;

    float m_Gravity = 200.f;

    // Speed variables for Sprinting
    float m_WalkSpeed = 200.f;
    float m_SprintSpeed = 350.f;
    float m_CurrentSpeed = 150.f; // The actual speed used in calculations

    InputReceiver m_InputReceiver;

    sf::Clock m_JumpClock;
    bool m_SpaceHeldDown = false;

    float m_JumpDuration = 0.60f; // let them rise a bit longer while holding
    float m_JumpSpeed = 650.f;    // much stronger initial jump

    // Double Jump Variables
    int m_JumpsRemaining = 0;
    const int MAX_JUMPS = 2;
    bool m_JumpKeyReleased = true; // Prevents holding space to fly

    // Flight Powerup
    bool m_FlightActive = false;
    float m_FlightDuration = 3.f; // seconds
    sf::Clock m_FlightClock;
    float m_FlightSpeed = 250.f; // how fast to move up/down while flying

    // Health
    int m_MaxHealth = 10;
    int m_Health = m_MaxHealth;
    bool m_IsInvincible = false;
    float m_InvincibleTime = 1.0f; // seconds of "grace" after a hit
    sf::Clock m_InvincibleClock;

    // Action Flags
    bool m_IsDucking = false;
    bool m_IsDropping = false;
    bool m_WasDucking = false; // To detect the transition for hitbox resizing

  public:
    bool m_RightIsHeldDown = false;
    bool m_LeftIsHeldDown = false;

    bool m_SprintIsHeldDown = false;

    bool m_IsGrounded = false;
    bool m_InJump = false;

    // Climbing
    bool m_IsClimbing = false;
    float m_ClimbSpeed = 160.f;
    float m_ClimbAnchorX = 0.f;

    sf::FloatRect *getPositionPointer();
    bool *getGroundedPointer();
    InputReceiver *getInputReceiver();

    void handleInput();

    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    void update(float fps) override;

    // Flight
    void grantFlight(float seconds);
    bool isFlightActive() const
    {
        return m_FlightActive;
    }

    // Climbing
    void startClimbing(float anchorX)
    {
        m_IsClimbing = true;
        m_ClimbAnchorX = anchorX;
        m_InJump = false; // cancel jump state
        m_JumpsRemaining = MAX_JUMPS;
    }
    void stopClimbing()
    {
        m_IsClimbing = false;
    }

    // Health
    int getHealth() const
    {
        return m_Health;
    }
    void resetHealth()
    {
        m_Health = m_MaxHealth;
        m_IsInvincible = false;
    }
    int getMaxHealth() const
    {
        return m_MaxHealth;
    }
    bool isDead() const
    {
        return m_Health <= 0;
    }
    void takeDamage(int amount);

    // Getters for new actions
    bool isDucking() const
    {
        return m_IsDucking;
    }
    bool isDropping() const
    {
        return m_IsDropping;
    }

    // Getter for Hurt animation
    bool isInvincible() const
    {
        return m_IsInvincible;
    }
};