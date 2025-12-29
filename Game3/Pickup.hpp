#pragma once
#include <SFML/Graphics.hpp>

class Pickup
{
  public:
    Pickup(int type);
    // Prepare a new pickup
    void setArena(sf::IntRect arena);
    void spawn();
    // Check the position of a pickup
    sf::FloatRect getPosition();
    // Get the sprite for drawing
    sf::Sprite getSprite();
    // Let the pickup update itself each frame
    void update(float elapsedTime);
    // Is this pickup currently spawned?
    bool isSpawned();
    // Get the goodness from the pickup
    int gotIt();
    // Upgrade the value of each pickup
    void upgrade();

  private:
    // Start value for health pickups
    const int HEALTH_START_VALUE = 50;
    const int AMMO_START_VALUE = 12;
    const int START_WAIT_TIME = 10;
    const int START_SECONDS_TO_LIVE = 5;

    // The sprite that represents this pickup
    sf::Sprite m_Sprite;
    // The arena it exists in
    sf::IntRect m_Arena;
    // How much is this pickup worth?
    int m_Value;

    // What type of pickup is this?
    // 1 = health, 2 = ammo
    int m_Type;
    // Handle spawning and disappearing
    bool m_Spawned;
    float m_SecondsSinceSpawn;
    float m_SecondsSinceDeSpawn;
    float m_SecondsToLive;
    float m_SecondsToWait;
};
