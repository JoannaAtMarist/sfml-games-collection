#include "Pickup.hpp"
#include "TextureHolder.hpp"

using namespace sf;
using namespace std;

// Constructor
Pickup::Pickup(int type) : m_Type{type}
{
    // Associate the texture with the sprite
    if (m_Type == 1) {
        m_Sprite = Sprite(
            TextureHolder::GetTexture("graphics/pickups/health-red 32px.png"));
        // How much is pickup worth
        m_Value = HEALTH_START_VALUE;
    } else {
        m_Sprite = Sprite(
            TextureHolder::GetTexture("graphics/pickups/ammo-pistol 32px.png"));
        // How much is pickup worth
        m_Value = AMMO_START_VALUE;
    }
    m_Sprite.setOrigin(25, 25);
    m_SecondsToLive = START_SECONDS_TO_LIVE;
    m_SecondsToWait = START_WAIT_TIME;
}

// Prepare a new pickup
void Pickup::setArena(IntRect arena)
{
    // Copy the details of the arena to the pickup's m_Arena
    m_Arena.left = arena.left + 50;
    m_Arena.width = arena.width - 50;
    m_Arena.top = arena.top + 50;
    m_Arena.height = arena.height - 50;
    spawn();
}

// Spawn a new pickup
void Pickup::spawn()
{
    // Spawn at a random location
    srand((int)time(0) / m_Type);
    int x = (rand() % m_Arena.width);
    srand((int)time(0) * m_Type);
    int y = (rand() % m_Arena.height);
    m_SecondsSinceSpawn = 0;
    m_Spawned = true;
    m_Sprite.setPosition(x, y);
}

// Check the position of a pickup
FloatRect Pickup::getPosition()
{
    return m_Sprite.getGlobalBounds();
}

// Get the sprite for drawing
Sprite Pickup::getSprite()
{
    return m_Sprite;
}

// Is this pickup currently spawned?
bool Pickup::isSpawned()
{
    return m_Spawned;
}

// Got it?
int Pickup::gotIt()
{
    m_Spawned = false;
    m_SecondsSinceDeSpawn = 0;
    return m_Value;
}

// Let the pickup update itself each frame
void Pickup::update(float elapsedTime)
{
    if (m_Spawned) {
        m_SecondsSinceSpawn += elapsedTime;
    } else {
        m_SecondsSinceDeSpawn += elapsedTime;
    }
    // Do we need to hide a pickup?
    if (m_SecondsSinceSpawn > m_SecondsToLive && m_Spawned) {
        // Remove the pickup and put it somewhere else
        m_Spawned = false;
        m_SecondsSinceDeSpawn = 0;
    }
    // Do we need to spawn a pickup
    if (m_SecondsSinceDeSpawn > m_SecondsToWait && !m_Spawned) {
        // spawn the pickup and reset the timer
        spawn();
    }
}

// Upgrade the value of each pickup
void Pickup::upgrade()
{
    if (m_Type == 1) {
        m_Value += (HEALTH_START_VALUE * .5);
    } else {
        m_Value += (AMMO_START_VALUE * .5);
    }
    // Make them more frequent and last longer
    m_SecondsToLive += (START_SECONDS_TO_LIVE / 10);
    m_SecondsToWait -= (START_WAIT_TIME / 10);
}
