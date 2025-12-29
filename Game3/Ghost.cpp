#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include "Ghost.hpp"

#include "TextureHolder.hpp"

using namespace sf;
using namespace std;

// Sprite scaling
float scale = 3.f;

// Spawn a new Ghost
void Ghost::spawn(float startX, float startY, int type, int seed)
{
    // Remember what type this ghost is
    m_Type = type;
    m_Enraged = false;

    switch (type) {
        case 0: // White Ghost - Vanilla

            m_Sprite = sf::Sprite(TextureHolder::GetTexture(
                "graphics/ghosts/ghost_attack_white.png"));
            // Arguments: (x, y, width, height)
            m_Sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
            m_Sprite.setScale(scale, scale);

            m_Speed = WHITE_SPEED;
            m_Health = WHITE_HEALTH;

            break;
        case 1: // Green Ghost - Slimer Type

            m_Sprite = Sprite(TextureHolder::GetTexture(
                "graphics/ghosts/ghost_attack_green.png"));
            m_Sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
            m_Sprite.setScale(4.f, 4.f);

            m_Speed = GREEN_SPEED;
            m_Health = GREEN_HEALTH;

            break;
        case 2: // Blue Ghost - Speedy

            m_Sprite = Sprite(TextureHolder::GetTexture(
                "graphics/ghosts/ghost_attack_blue.png"));
            m_Sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
            m_Sprite.setScale(scale, scale);

            m_Speed = BLUE_SPEED;
            m_Health = BLUE_HEALTH;

            break;
        case 3: // Red Ghost - Evil Jerk

            m_Sprite = Sprite(TextureHolder::GetTexture(
                "graphics/ghosts/ghost_attack_red.png"));
            m_Sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
            m_Sprite.setScale(scale, scale);

            m_Speed = RED_SPEED;
            m_Health = RED_HEALTH;

            break;
    }

    // Modify the speed to make the Ghost unique
    // Every Ghost is unique. Create a speed modifier
    srand((int)time(0) * seed);
    // Somewhere between 80 and 100
    float modifier = (rand() % MAX_VARIANCE) + OFFSET;
    // Express this as a fraction of 1
    modifier /= 100.f; // Now equals between .7 and 1
    m_Speed *= modifier;

    // Initialize its location
    m_Position.x = startX;
    m_Position.y = startY;
    // Set its origin to its center
    m_Sprite.setOrigin(25, 25);
    // Set its position
    m_Sprite.setPosition(m_Position);

    // ghost is now in play
    m_Active = true;
}

// Handle when a bullet hits a Ghost
bool Ghost::hit()
{
    m_Health--;
    if (m_Health < 0) {
        // gone
        m_Active = false;
        m_Sprite.setTexture(
            TextureHolder::GetTexture("graphics/ghosts/ghost_exp_white.png"));
        m_Sprite.setTextureRect(sf::IntRect(128, 0, 64, 64));
        m_Sprite.setScale(scale, scale);
        return true;
    }
    // injured but not gone yet
    return false;
}

// Find out if the Ghost is Active
bool Ghost::isActive()
{
    return m_Active;
}

// Return a rectangle that is the position in the world
FloatRect Ghost::getPosition()
{
    return m_Sprite.getGlobalBounds();
}

// Get a copy of the sprite to draw
Sprite Ghost::getSprite()
{
    return m_Sprite;
}

// Update the Ghost each frame
void Ghost::update(float elapsedTime, Vector2f playerLocation)
{
    static int counter = 0;
    if (counter < 60) {
        counter++;
    }

    float playerX = playerLocation.x;
    float playerY = playerLocation.y;

    // Special behavior: Red ghosts ENRAGE when close or wounded
    if (m_Type == 3 && !m_Enraged) {
        float dx = playerX - m_Position.x;
        float dy = playerY - m_Position.y;
        float dist2 = dx * dx + dy * dy;

        const float ENRAGE_DISTANCE = 250.f;

        // Either close enough OR at half health or less
        if (dist2 < ENRAGE_DISTANCE * ENRAGE_DISTANCE ||
            m_Health <= (RED_HEALTH / 2.f)) {

            m_Speed *= 2.0f; // go much faster
            m_Enraged = true;
            m_Sprite.setColor(sf::Color(255, 80, 80)); // extra red tint

            std::cout << "[DEBUG] Red ghost enraged! New speed = " << m_Speed
                      << std::endl;
        }
    } // End Enrage

    // Move horizontally toward the player
    if (playerX > m_Position.x) {
        m_Position.x += m_Speed * elapsedTime;
    }
    if (playerX < m_Position.x) {
        m_Position.x -= m_Speed * elapsedTime;
    }

    // Move vertically toward the player
    if (playerY > m_Position.y) {
        m_Position.y += m_Speed * elapsedTime;
    }
    if (playerY < m_Position.y) {
        m_Position.y -= m_Speed * elapsedTime;
    }

    // Move the sprite
    m_Sprite.setPosition(m_Position);

    // Face the sprite toward the player
    float angle =
        (atan2(playerY - m_Position.y, playerX - m_Position.x) * 180.f) /
        3.141f;
    m_Sprite.setRotation(angle);
}

// types
int Ghost::getType() const
{
    return m_Type;
}