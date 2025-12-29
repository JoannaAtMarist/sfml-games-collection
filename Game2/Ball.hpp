#pragma once
#include <SFML/Graphics.hpp>

// Ball class represents the "head" that bounces around the play field.
// Handles movement, collisions with screen edges, and special power-up effects.
class Ball
{

  private:
    sf::Sprite sprite;     // The visible sprite (zombie head)
    sf::Vector2f position; // Current position of the ball

    float directionX = 0.2f; // Current horizontal direction (positive = right,
                             // negative = left)
    float directionY =
        0.2f; // Current vertical direction (positive = down, negative = up)

    float normalSpeed = 1600.f; // Default speed of the ball
    float speed;                // Current speed (can be boosted by power-ups)

    // Power-up state
    bool headSpinActive = false; // True when Head Spin is active
    float headSpinTimer = 0.0f;  // Time remaining for Head Spin effect

    // Hit tracking (to prevent double scoring on same collision)
    bool justHit = false;

  public:
    // Constructor: set initial position and sprite texture
    Ball(float startX, float startY, sf::Texture const &texture);

    // Get the ball's bounding box (for collision detection)
    sf::FloatRect getPosition();

    // Return the current X direction (for logic/debug)
    float getXVelocity();

    // Collision reactions (reverse direction appropriately)
    void reboundSides();
    void reboundTop();
    void reboundBottom();
    void reboundLeft();
    void reboundRight();

    // Reset ball to a new position with a random direction
    void reset(float startX, float startY);

    // Update ball position each frame and handle power-up timers
    void update(sf::Time dt, float windowWidth, float windowHeight);

    // Ball setter
    void setDirection(float dx, float dy);

    // Draw the ball to the render window
    void draw(sf::RenderWindow &window);

    // Activate Head Spin power-up: makes ball faster and red
    void activateHeadSpin(float duration, float speedMultiplier);

    // Manage "just hit" flag to avoid duplicate scoring
    bool hasJustHit() const;
    void setJustHit(bool value);
};
