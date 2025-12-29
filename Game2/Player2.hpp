#pragma once
#include <SFML/Graphics.hpp>
#include "Orientation.hpp"

// Player2 represents the Dog paddle on the right side.
// Supports sprite-sheet textures, scaling, and ghost ability.
// Uses Arrow keys (or IJKL) for input in main.cpp.
class Player2 {

private:
    sf::Sprite sprite;        // The visible sprite (dog)
    sf::Vector2f position;    // Current position
    float speed = 600.f;      // Movement speed

    // Movement flags (true when key is pressed, updated each frame)
    bool movingNegative = false; // Up or Left depending on orientation
    bool movingPositive = false; // Down or Right depending on orientation

    Orientation orientation;  // Movement axis (Vertical or Horizontal)

public:
    // Constructor: initialize with optional sprite sheet rect and custom scaling
    Player2(float startX, float startY, Orientation o,
        sf::Texture const& texture, sf::IntRect rect = sf::IntRect(),
        float scaleX = 1.f, float scaleY = 1.f);

    // Get bounding box (for collision detection with ball)
    sf::FloatRect getPosition();

    // Input controls (set movement flags)
    void moveNegative();  // Move up/left
    void movePositive();  // Move down/right
    void stopNegative();  // Stop moving up/left
    void stopPositive();  // Stop moving down/right

    // Update paddle position each frame based on input + boundaries
    void update(sf::Time dt, float windowWidth, float windowHeight);

    // Force set Y position (used by ghost dog helper)
    void setY(float y);

    // Draw paddle to the render window
    void draw(sf::RenderWindow& window);

    // Utility: allow external code to change appearance
    void setScale(float x, float y);
    void setColor(const sf::Color& color);
};
