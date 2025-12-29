#pragma once
#include <SFML/Graphics.hpp>

class Bullet
{
  public:
    Bullet();

    void stop();
    bool isInFlight();

    // Launch a new bullet
    void shoot(float startX, float startY, float xTarget, float yTarget);

    // Collision box in world space
    sf::FloatRect getPosition();

    // Sprite for drawing
    sf::Sprite &getSprite();

    // Update movement + animation
    void update(float elapsedTime);

  private:
    // Position and movement
    sf::Vector2f m_Position;
    sf::Vector2f m_StartPosition;
    float m_BulletSpeed = 1000.0f;
    float m_BulletDistanceX = 0.0f;
    float m_BulletDistanceY = 0.0f;
    bool m_InFlight = false;

    // Simple range limit
    float m_MinX = 0.0f;
    float m_MaxX = 0.0f;
    float m_MinY = 0.0f;
    float m_MaxY = 0.0f;

    // Distance-based lifetime (for safety)
    float m_DistanceTravelled = 0.0f;
    float m_MaxDistance = 1000.0f; // adjust for longer/shorter beams

    // Visuals
    sf::Sprite m_BulletSprite;

    // Animation state
    int m_CurrentFrame = 0;
    float m_TimeSinceFrame = 0.0f;
    float m_FrameTime = 0.05f; // seconds per frame (20 fps)
};
