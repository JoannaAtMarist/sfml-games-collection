#include <cmath>
#include <iostream>
#include "Player.hpp"

using namespace sf;
using namespace std;

// Constructor
Player::Player()
    : m_Speed(START_SPEED), m_Health(START_HEALTH), m_MaxHealth(START_HEALTH),
      m_Texture(), m_Sprite()
{
    // Associate a texture with the sprite
    m_Sprite = Sprite(TextureHolder::GetTexture("graphics/Holtz.png"));

    // Set the origin of the sprite to the center,
    // for smooth rotation
    m_Sprite.setOrigin(25, 25);
}

// Spawn the player in the arena
void Player::spawn(IntRect arena, Vector2f resolution, int tileSize)
{
    // Place the player in the middle of the arena
    m_Position.x = arena.width / 2;
    m_Position.y = arena.height / 2;
    // Copy the details of the arena to the player's m_Arena
    m_Arena.left = arena.left;
    m_Arena.width = arena.width;
    m_Arena.top = arena.top;
    m_Arena.height = arena.height;
    // Remember how big the tiles are in this arena
    m_TileSize = tileSize;
    // Store the resolution for future use
    m_Resolution.x = resolution.x;
    m_Resolution.y = resolution.y;
}

// Reset player stats
void Player::resetPlayerStats()
{
    m_Speed = START_SPEED;
    m_Health = START_HEALTH;
    m_MaxHealth = START_HEALTH;
}

// Handle the player getting hit by a ghost
bool Player::hit(Time timeHit, int damage)
{
    if (timeHit.asMilliseconds() - m_LastHit.asMilliseconds() > 200) {
        m_LastHit = timeHit;
        m_Health -= damage;
        return true;
    } else {
        return false;
    }
}

// How long ago was the player last hit
Time Player::getLastHitTime()
{
    return m_LastHit;
}

// Set the center
void Player::setCenter(Vector2f newCenter)
{
    m_Position = newCenter;
    m_Sprite.setPosition(m_Position);
}

// We will call this function once every frame
void Player::update(float elapsedTime, Vector2i mousePosition)
{
    // Dash timers
    if (m_DashCooldownRemaining > 0.0f) {
        m_DashCooldownRemaining -= elapsedTime;
        if (m_DashCooldownRemaining < 0.0f) {
            m_DashCooldownRemaining = 0.0f;
        }
    }

    if (m_IsDashing) {
        m_DashTimeRemaining -= elapsedTime;
        if (m_DashTimeRemaining <= 0.0f) {
            m_IsDashing = false;
            m_DashTimeRemaining = 0.0f;
        }
    }

    // Base movement speed, optionally boosted by dash
    float currentSpeed = m_Speed;
    if (m_IsDashing) {
        currentSpeed *= m_DashSpeedMultiplier;
    }

    // Updated movement logic to use currentSpeed
    if (m_UpPressed) {
        m_Position.y -= currentSpeed * elapsedTime;
    }
    if (m_DownPressed) {
        m_Position.y += currentSpeed * elapsedTime;
    }
    if (m_RightPressed) {
        m_Position.x += currentSpeed * elapsedTime;
    }
    if (m_LeftPressed) {
        m_Position.x -= currentSpeed * elapsedTime;
    }

    // Keep the player in the arena
    if (m_Position.x > m_Arena.width - m_TileSize) {
        m_Position.x = m_Arena.width - m_TileSize;
    }
    if (m_Position.x < m_Arena.left + m_TileSize) {
        m_Position.x = m_Arena.left + m_TileSize;
    }
    if (m_Position.y > m_Arena.height - m_TileSize) {
        m_Position.y = m_Arena.height - m_TileSize;
    }
    if (m_Position.y < m_Arena.top + m_TileSize) {
        m_Position.y = m_Arena.top + m_TileSize;
    }

    // Move the sprite to the new position
    m_Sprite.setPosition(m_Position);

    // Calculate the angle the player is facing (mouse pointer)
    float angle = (atan2(mousePosition.y - m_Resolution.y / 2,
                         mousePosition.x - m_Resolution.x / 2) *
                   180) /
                  3.141;
    m_Sprite.setRotation(angle);
}

// Give the player a speed boost
void Player::upgradeSpeed()
{
    // 20% speed upgrade
    m_Speed += (START_SPEED * .2);
}

// Give the player some health
void Player::upgradeHealth()
{
    // 20% max health upgrade
    m_MaxHealth += (START_HEALTH * .2);
}

// Increase the max health the player can have
void Player::increaseHealthLevel(int amount)
{
    m_Health += amount;
    // But not beyond the maximum
    if (m_Health > m_MaxHealth) {
        m_Health = m_MaxHealth;
    }
}

// Dash Ability
void Player::triggerDash()
{
    // Only start a dash if we are not already dashing
    // and the cooldown has expired
    if (!m_IsDashing && m_DashCooldownRemaining <= 0.0f) {
        m_IsDashing = true;
        m_DashTimeRemaining = m_DashDuration;
        m_DashCooldownRemaining = m_DashCooldown;
    }
}

// Movement functions
void Player::moveLeft()
{
    m_LeftPressed = true;
}
void Player::moveRight()
{
    m_RightPressed = true;
}
void Player::moveUp()
{
    m_UpPressed = true;
}
void Player::moveDown()
{
    m_DownPressed = true;
}
void Player::stopLeft()
{
    m_LeftPressed = false;
}
void Player::stopRight()
{
    m_RightPressed = false;
}
void Player::stopUp()
{
    m_UpPressed = false;
}
void Player::stopDown()
{
    m_DownPressed = false;
}

// Where is the player
FloatRect Player::getPosition()
{
    return m_Sprite.getGlobalBounds();
}
Vector2f Player::getCenter()
{
    return m_Position;
}
float Player::getRotation()
{
    return m_Sprite.getRotation();
}

// Send a copy of the sprite to the main function
Sprite Player::getSprite()
{
    return m_Sprite;
}

// How much health has the player currently got?
int Player::getHealth()
{
    return m_Health;
}