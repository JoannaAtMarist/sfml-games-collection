#include "Ball.hpp"
#include <iostream>

// Constructor: creates a Ball object with a texture and start position
// Initializes sprite, scales it down, and sets normal speed
Ball::Ball(float startX, float startY, sf::Texture const &texture)
    : position(startX, startY)
{
    sprite.setTexture(texture);

    // Shrink the ball/zombie head sprite to half size
    sprite.setScale(0.5f, 0.5f);

    sprite.setPosition(position);
    sprite.setTexture(texture);
    speed = normalSpeed;
}

// Return the current bounding box of the ball (for collision checks)
sf::FloatRect Ball::getPosition()
{
    return sprite.getGlobalBounds();
}

// Return the X velocity direction (used for game logic/debugging)
float Ball::getXVelocity()
{
    return directionX;
}

// Invert directions when bouncing off walls or bats/players
void Ball::reboundSides()
{
    directionX = -directionX;
}
void Ball::reboundTop()
{
    directionY = -directionY;
}
void Ball::reboundBottom()
{
    directionY = -directionY;
}
void Ball::reboundLeft()
{
    directionX = -directionX;
}
void Ball::reboundRight()
{
    directionX = -directionX;
}

// Reset the ball to the center with a new random direction
void Ball::reset(float startX, float startY)
{
    position.x = startX;
    position.y = startY;
    sprite.setPosition(position);

    // Randomize whether it starts left/right and up/down
    directionX = (rand() % 2 == 0) ? -0.2f : 0.2f;
    directionY = (rand() % 2 == 0) ? -0.2f : 0.2f;
}

// Update ball position each frame
// Handles screen boundary bounces and active power-up timer
void Ball::update(sf::Time dt, float windowWidth, float windowHeight)
{
    // Move based on direction, speed, and frame time
    position.x += directionX * speed * dt.asSeconds();
    position.y += directionY * speed * dt.asSeconds();
    sprite.setPosition(position);

    // Bounce off left/right edges
    if (position.x < 0)
        reboundLeft();
    if (position.x + sprite.getGlobalBounds().width > windowWidth)
        reboundRight();

    // Bounce off top/bottom edges
    if (position.y < 0)
        reboundTop();
    if (position.y + sprite.getGlobalBounds().height > windowHeight)
        reboundBottom();

    // If Head Spin power-up is active, count down its timer
    if (headSpinActive) {
        headSpinTimer -= dt.asSeconds();
        if (headSpinTimer <= 0) {
            headSpinActive = false;
            speed = normalSpeed;
            sprite.setColor(sf::Color::White); // restore default look
        }
    }
}

// Ball setter
void Ball::setDirection(float dx, float dy)
{
    directionX = dx;
    directionY = dy;
}

// Draw the ball sprite to the window
void Ball::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

// Activate the Head Spin power-up
// Makes the ball faster and red for a set duration
void Ball::activateHeadSpin(float duration, float speedMultiplier)
{
    std::cout << "Head Spin activated!\n";

    headSpinActive = true;
    headSpinTimer = duration;

    speed = normalSpeed * speedMultiplier;
    sprite.setColor(sf::Color::Red);
}

// Manage "just hit" flag to avoid duplicate scoring
bool Ball::hasJustHit() const
{
    return justHit;
}
// Set the "just hit" flag
void Ball::setJustHit(bool value)
{
    justHit = value;
}