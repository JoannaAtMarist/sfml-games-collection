#pragma once
#include <SFML/Graphics.hpp>
#include "TextureHolder.hpp"

class Player
{
  public:
    Player();

    void spawn(sf::IntRect arena, sf::Vector2f resolution, int tileSize);

    // Call this at the end of every game
    void resetPlayerStats();

    // Handle the player getting hit by a ghost
    bool hit(sf::Time timeHit, int damage = 10);
    // How long ago was the player last hit
    sf::Time getLastHitTime();

    // Where is the player
    sf::FloatRect getPosition();
    // Where is the center of the player
    sf::Vector2f getCenter();
    // Set the center
    void setCenter(sf::Vector2f newCenter);
    // What angle is the player facing
    float getRotation();

    // Send a copy of the sprite to the main function
    sf::Sprite getSprite();
    // The next four functions move the player
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    // Stop the player moving in a specific direction
    void stopLeft();
    void stopRight();
    void stopUp();
    void stopDown();

    // We will call this function once every frame
    void update(float elapsedTime, sf::Vector2i mousePosition);

    // Give the player a speed boost
    void upgradeSpeed();
    // Give the player some health
    void upgradeHealth();
    // Increase the max health the player can have
    void increaseHealthLevel(int amount);
    // How much health has the player currently got?
    int getHealth();

    // Dash Ability
    void triggerDash(); // asks player to start a dash if cooldown allows
    bool isDashing() const
    {
        return m_IsDashing;
    }

  private:
    const float START_SPEED = 200;
    const float START_HEALTH = 100;

    // Where is the player
    sf::Vector2f m_Position;

    // The sprite
    sf::Sprite m_Sprite;
    // And a texture
    sf::Texture m_Texture;

    // What is the screen resolution
    sf::Vector2f m_Resolution;
    // What size is the current arena
    sf::IntRect m_Arena;
    // How big is each tile of the arena
    int m_TileSize;

    // Which direction(s)the player is moving in
    bool m_UpPressed;
    bool m_DownPressed;
    bool m_LeftPressed;
    bool m_RightPressed;

    // How much health has the player got?
    int m_Health;
    // What is the max health the player can have
    int m_MaxHealth;
    // When was the player last hit
    sf::Time m_LastHit;
    // Speed in pixels per second
    float m_Speed;

    // Dash ability
    bool m_IsDashing = false;
    float m_DashTimeRemaining = 0.0f;     // seconds left in current dash
    float m_DashDuration = 0.20f;         // how long a dash lasts
    float m_DashCooldownRemaining = 0.0f; // seconds until dash is ready again
    float m_DashCooldown = 3.0f;          // time between dashes
    float m_DashSpeedMultiplier = 3.0f;   // speed boost while dashing
};
