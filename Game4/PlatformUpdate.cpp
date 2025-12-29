#include "PlatformUpdate.hpp"
#include "PlayerUpdate.hpp"
#include "LevelUpdate.hpp"

using namespace sf;
using namespace std;

// Access to the underlying rectangle (for legacy uses)
FloatRect *PlatformUpdate::getPositionPointer()
{
    return &m_Position;
}

// Simple position/size helpers for graphics
void PlatformUpdate::setPosition(float x, float y)
{
    m_Position.left = x;
    m_Position.top = y;
}

// Get position as vector
Vector2f PlatformUpdate::getPosition() const
{
    return {m_Position.left, m_Position.top};
}

// Set size
void PlatformUpdate::setSize(float w, float h)
{
    m_Position.width = w;
    m_Position.height = h;
}

// Get size as vector
Vector2f PlatformUpdate::getSize() const
{
    return {m_Position.width, m_Position.height};
}

// Assemble pointers to player and level
void PlatformUpdate::assemble(shared_ptr<LevelUpdate> levelUpdate,
                              shared_ptr<PlayerUpdate> playerUpdate)
{
    m_LevelUpdate = levelUpdate.get();

    if (playerUpdate) {
        m_PlayerPosition = playerUpdate->getPositionPointer();
        m_PlayerIsGrounded = playerUpdate->getGroundedPointer();
        m_Player = playerUpdate.get(); // save raw pointer
    }
}

// Update logic
void PlatformUpdate::update(float dt)
{
    if (!m_PlayerPosition || !m_Active)
        return;

    //. DISAPPEARING LOGIC

    // If the platform is set to disappear, and the player has stood on it,
    // start counting time
    if (m_Disappearing && m_TimeOnPlatform > 0.f) {
        m_TimeOnPlatform += dt;

        // Visual debug: print to console when it's ticking
        std::cout << "Platform dissolving: " << m_TimeOnPlatform << std::endl;

        if (m_TimeOnPlatform >= m_DisappearDelay) {
            m_Active = false; // Poof!
            return;           // Stop processing collision
        }
    }

    // CLIMBING IGNORE
    // If the player is climbing, do NOT collide with platforms.
    // This allows them to climb "behind" or "through" platforms on the tree.
    if (m_Player && m_Player->m_IsClimbing) {
        return;
    }

    // Reset flag for this frame (will be set true below if collision happens)
    bool wasPlayerOnThis = m_PlayerOnThis;
    m_PlayerOnThis = false;

    //. COLLISION LOGIC
    if (m_Position.intersects(*m_PlayerPosition)) {

        // DROP-THROUGH CHECK
        // If player is holding S + Space, ignore this collision entirely
        if (m_Player && m_Player->isDropping()) {
            return;
        }

        float playerFeetY = m_PlayerPosition->top + m_PlayerPosition->height;

        // Check if landing on top
        bool fallingDown = playerFeetY > m_Position.top;
        bool landedOnTop = (m_PlayerPosition->top < m_Position.top);

        // We also check m_Player->getGroundedPointer() to see if we are already
        // sitting on it (Because if we are standing still, fallingDown might be
        // false due to float precision)

        if ((fallingDown && landedOnTop) || (wasPlayerOnThis && landedOnTop)) {

            // 👣
            m_PlayerPosition->top = m_Position.top - m_PlayerPosition->height;
            *m_PlayerIsGrounded = true;
            m_PlayerOnThis = true;

            // 1. TRIGGER DISAPPEARING TIMER
            if (m_Disappearing) {
                // Start the timer if it hasn't started yet
                if (m_TimeOnPlatform == 0.f) {
                    m_TimeOnPlatform = 0.001f; // Initialize to non-zero
                }
            }

            // 2. TRIGGER DEADLY
            // check the level before killing the player
            if (m_Deadly && m_Player) {
                bool kill = true;
                // If a specific level is required for deadlines, check it
                if (m_DeadlyLevel != 0 && m_LevelUpdate) {
                    if (m_LevelUpdate->getCurrentLevel() != m_DeadlyLevel) {
                        kill = false;
                    }
                }

                if (kill) {
                    m_Player->takeDamage(m_Player->getMaxHealth());
                }
            }

            // 3. FLIGHT
            if (m_GrantsFlight && m_Player) {
                m_Player->grantFlight(m_FlightSeconds);
            }
        }
    }
}

// Set whether this platform grants flight, and for how long
void PlatformUpdate::setGrantsFlight(bool grants, float seconds)
{
    m_GrantsFlight = grants;
    m_FlightSeconds = seconds;
}

// Set whether this platform is deadly to the player
void PlatformUpdate::setDeadly(bool deadly, int activeLevel)
{
    m_Deadly = deadly;
    m_DeadlyLevel = activeLevel;
}

// Set whether this platform disappears after being stood on
void PlatformUpdate::setDisappearing(bool disappearing, float delaySeconds)
{
    m_Disappearing = disappearing;
    m_DisappearDelay = delaySeconds;
}

// Check if platform is active (for collision and drawing)
bool PlatformUpdate::isActive() const
{
    return m_Active;
}

// Reset logic for when the platform is recycled or level restarts
void PlatformUpdate::reset()
{
    m_Active = true;
    m_TimeOnPlatform = 0.f;
    m_PlayerOnThis = false;
}