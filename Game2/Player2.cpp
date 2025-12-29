#include "Player2.hpp"
#include <iostream>

// Constructor: initializes Player2 (Dog paddle)
// Can take a texture rect (from sprite sheet) and custom scale
Player2::Player2(float startX, float startY, Orientation o, 
    sf::Texture const& texture, sf::IntRect rect,
    float scaleX, float scaleY)
    : position(startX, startY), orientation(o)
{
    sprite.setTexture(texture);

    // Optional: use a sub-rectangle from a sprite sheet
    if (rect != sf::IntRect()) {
        sprite.setTextureRect(rect);  // pick from sprite sheet
    }

    // Scale sprite (e.g., enlarge the dog)
    sprite.setScale(scaleX, scaleY);

    // Center origin for natural movement
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(position);
}

// Return bounding box for collision checks
sf::FloatRect Player2::getPosition() { return sprite.getGlobalBounds(); }

// Movement control flags (applied in update)
void Player2::moveNegative() { movingNegative = true; } // Up or Left
void Player2::movePositive() { movingPositive = true; } // Down or Right
void Player2::stopNegative() { movingNegative = false; }
void Player2::stopPositive() { movingPositive = false; }

// Update paddle position based on controls, orientation, and boundaries
void Player2::update(sf::Time dt, float windowWidth, float windowHeight) {
    if (orientation == Orientation::Horizontal) {
        // Left/right movement
        if (movingNegative) position.x -= speed * dt.asSeconds();
        if (movingPositive) position.x += speed * dt.asSeconds();

        // Clamp to screen edges
        if (position.x < 0) position.x = 0;
        if (position.x + sprite.getGlobalBounds().width > windowWidth)
            position.x = windowWidth - sprite.getGlobalBounds().width;
    }
    else { // Vertical paddle
        if (movingNegative) position.y -= speed * dt.asSeconds();
        if (movingPositive) position.y += speed * dt.asSeconds();

        // Vertical paddle
        float halfHeight = sprite.getGlobalBounds().height / 2.f;
        if (position.y < halfHeight)
            position.y = halfHeight;
        if (position.y > windowHeight - halfHeight)
            position.y = windowHeight - halfHeight;

    }
    sprite.setPosition(position);
}

// Set paddle Y directly (used by ghost helper dog)
void Player2::setY(float y) {
    position.y = y;
    sprite.setPosition(position);
}

// Draw sprite to the window
void Player2::draw(sf::RenderWindow& window) { window.draw(sprite); }

// Utility: change scale or color (for power-ups or effects)
void Player2::setScale(float x, float y) { sprite.setScale(x, y); }
void Player2::setColor(const sf::Color& color) { sprite.setColor(color); }