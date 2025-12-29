#include <cmath>
#include "BeeProjectileUpdate.hpp"
#include "LevelUpdate.hpp"
#include "PlayerUpdate.hpp"

using namespace sf;

// Constructor
BeeProjectileUpdate::BeeProjectileUpdate() = default;

// In assemble(), store the LevelUpdate pointer
void BeeProjectileUpdate::assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                                   std::shared_ptr<PlayerUpdate> playerUpdate)
{
    m_LevelUpdate = levelUpdate.get();

    if (playerUpdate) {
        m_PlayerRect = playerUpdate->getPositionPointer();
        m_Player = playerUpdate.get();
    }
}

// Set initial position
void BeeProjectileUpdate::setPosition(float x, float y)
{
    m_Bounds.left = x;
    m_Bounds.top = y;
    m_StartPos = sf::Vector2f(x, y); // store original spawn position
}

// Set velocity
void BeeProjectileUpdate::setVelocity(float vx, float vy)
{
    m_Velocity.x = vx;
    m_Velocity.y = vy;
}

// Enable zig-zag motion with given amplitude and speed
void BeeProjectileUpdate::enableZigZag(float amplitude, float speed)
{
    m_ZigZag = true;
    m_ZigAmplitude = amplitude;
    m_ZigSpeed = speed;
}

// Set damage amount
void BeeProjectileUpdate::setDamage(int dmg)
{
    m_Damage = dmg;
}

// Update method called each frame
void BeeProjectileUpdate::update(float dt)
{
    // Check Level
    if (m_ActiveLevel != 0 && m_LevelUpdate) {
        if (m_LevelUpdate->getCurrentLevel() != m_ActiveLevel) {
            // Hide the bee if it's the wrong level
            m_Bounds.left = -1000.f;
            return;
        }
    }

    // If somehow we don't have a player yet, don't do anything
    if (!m_PlayerRect || !m_Player)
        return;

    // 1) Handle "dead" / inactive bee: wait, then respawn

    if (!m_Active) {
        m_InactiveTime += dt;

        if (m_InactiveTime >= m_RespawnDelay) {
            // Reactivate and reset state
            m_Active = true;
            m_InactiveTime = 0.f;
            m_Time = 0.f;                 // reset zig-zag phase
            m_Bounds.left = m_StartPos.x; // go back to original spawn point
            m_Bounds.top = m_StartPos.y;
        }

        // While inactive we don't move or collide
        return;
    }

    // 2) Normal active behaviour (your existing logic)

    // Zig-zag bees
    m_Time += dt;

    // Move the bee (base straight-line motion)
    m_Bounds.left += m_Velocity.x * dt;
    m_Bounds.top += m_Velocity.y * dt;

    // Optional: zig-zag overlay on top of straight motion
    if (m_ZigZag) {
        float offset = std::sin(m_Time * m_ZigSpeed) * m_ZigAmplitude;
        m_Bounds.top += offset * dt; // small vertical wiggle
    }

    // Simple lifetime: if it goes way off-screen, deactivate
    if (m_Bounds.left > 4000.f || m_Bounds.left < -400.f) {
        m_Active = false;
        return;
    }

    // Collision with player
    if (m_Bounds.intersects(*m_PlayerRect)) {
        m_Player->takeDamage(m_Damage);
        m_Active = false; // will now go into the respawn timer path
    }
}
