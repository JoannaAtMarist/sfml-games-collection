#pragma once
#include <SFML/Graphics.hpp>

// Different types of Ghosts
enum GhostType
{
    GHOST_WHITE = 0,
    GHOST_GREEN = 1,
    GHOST_BLUE = 2,
    GHOST_RED = 3
};

class Ghost
{
  public:
    // Handle when a bullet hits a Ghost
    bool hit();
    // Find out if the Ghost is Active
    bool isActive();
    // Spawn a new Ghost
    void spawn(float startX, float startY, int type, int seed);
    // Return a rectangle that is the position in the world
    sf::FloatRect getPosition();
    // Get a copy of the sprite to draw
    sf::Sprite getSprite();
    // Update the Ghost each frame
    void update(float elapsedTime, sf::Vector2f playerLocation);

    // types
    int getType() const;

  private:
    // Ghost Stats
    // White Ghost - Vanilla
    const float WHITE_SPEED = 20;
    const float WHITE_HEALTH = 3;
    // Green Ghost - Slimer Type
    const float GREEN_SPEED = 40;
    const float GREEN_HEALTH = 5;
    // Blue Ghost - Speedy
    const float BLUE_SPEED = 80;
    const float BLUE_HEALTH = 1;
    // Red Ghost - Evil Jerk
    // (starts slow, then enrages)
    const float RED_SPEED = 25;
    const float RED_HEALTH = 6;

    // Make each Ghost vary its speed slightly
    const int MAX_VARIANCE = 30;
    const int OFFSET = 101 - MAX_VARIANCE;

    // Where is this Ghost?
    sf::Vector2f m_Position;
    // A sprite for the Ghost
    sf::Sprite m_Sprite;
    // How fast can this one run/crawl?
    float m_Speed;
    // How much health has it got?
    float m_Health;
    // Is it still Active?
    bool m_Active;

    // Type and behavior flags
    bool m_Enraged = false;
    int m_Type;
};
