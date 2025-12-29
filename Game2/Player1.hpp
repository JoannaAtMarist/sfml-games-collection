#pragma once
#include <SFML/Graphics.hpp>
#include "Orientation.hpp"

// Player1 represents the Zombie paddle on the left side.
// Can move vertically or horizontally depending on orientation.
// Uses WASD for input in main.cpp.
class Player1 {

private:
    sf::Sprite sprite;        // The visible sprite (zombie)
    sf::Vector2f position;    // Current position
    float speed = 600.f;      // Movement speed

    // Movement flags (true when key is pressed, updated each frame)
    bool movingNegative = false; // Up or Left depending on orientation
    bool movingPositive = false; // Down or Right depending on orientation

    Orientation orientation;  // Movement axis (Vertical or Horizontal)

public:
    // Constructor: place zombie paddle at starting position
    Player1(float startX, float startY, Orientation o, sf::Texture const& texture);

    // Get the paddle’s bounding box (for collisions with the ball)
    sf::FloatRect getPosition();

    // Input controls (set movement flags)
    void moveNegative();  // Move up/left
    void movePositive();  // Move down/right
    void stopNegative();  // Stop moving up/left
    void stopPositive();  // Stop moving down/right

    // Update paddle position each frame based on input + screen boundaries
    void update(sf::Time dt, float windowWidth, float windowHeight);

    // Draw paddle to the render window
    void draw(sf::RenderWindow& window);

    // Ability to set texture after game loads
    void setTexture(const sf::Texture& texture);
};
