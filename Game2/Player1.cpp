#include "Player1.hpp"
#include <iostream>

// Constructor: initializes Player1 (Zombie paddle) at given position
// Scales sprite down, centers origin, sets orientation
Player1::Player1(float startX, float startY, Orientation o, sf::Texture const& texture)
    : position(startX, startY), orientation(o) {
    sprite.setTexture(texture);

    // Shrink zombie to half size
    sprite.setScale(0.5f, 0.5f);

    // Center the origin so movement feels natural
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(position);
}

// Get the current bounding box of the paddle (for collision detection)
sf::FloatRect Player1::getPosition() { return sprite.getGlobalBounds(); }

// Movement controls (set booleans which are applied in update)
void Player1::moveNegative() { movingNegative = true; }
void Player1::movePositive() { movingPositive = true; }
void Player1::stopNegative() { movingNegative = false; }
void Player1::stopPositive() { movingPositive = false; }

// Update position based on movement flags, frame time, and screen boundaries
void Player1::update(sf::Time dt, float windowWidth, float windowHeight) {
    if (orientation == Orientation::Horizontal) {
        // Left/right movement
        if (movingNegative) position.x -= speed * dt.asSeconds();
        if (movingPositive) position.x += speed * dt.asSeconds();

        // Clamp to window width
        if (position.x < 0) position.x = 0;
        if (position.x + sprite.getGlobalBounds().width > windowWidth)
            position.x = windowWidth - sprite.getGlobalBounds().width;
    }
    else { // Vertical paddle
        if (movingNegative) position.y -= speed * dt.asSeconds();
        if (movingPositive) position.y += speed * dt.asSeconds();

        // Clamp to window height
        float halfHeight = sprite.getGlobalBounds().height / 2.f;
        if (position.y < halfHeight)
            position.y = halfHeight;
        if (position.y > windowHeight - halfHeight)
            position.y = windowHeight - halfHeight;

    }
    sprite.setPosition(position);
}

// Draw the paddle sprite to the window
void Player1::draw(sf::RenderWindow& window) { window.draw(sprite); }

// Ability to set texture after game loads
void Player1::setTexture(const sf::Texture& texture) { sprite.setTexture(texture); }
