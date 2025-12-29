#include "RoadUpdate.hpp"
#include "PlayerUpdate.hpp"
#include "LevelUpdate.hpp"

// Connect to player position and grounded state
void RoadUpdate::assemble(std::shared_ptr<LevelUpdate> level,
                          std::shared_ptr<PlayerUpdate> player)
{
    if (player) {
        m_PlayerPos = player->getPositionPointer();
        m_PlayerGrounded = player->getGroundedPointer();
    }
}

// Simple collision detection with the road
void RoadUpdate::update(float dt)
{
    // Safety check: ensure we have a reference to the player
    if (!m_PlayerPos)
        return;

    // Only check feet (simple ground)
    sf::Vector2f feet(m_PlayerPos->left + m_PlayerPos->width / 2,
                      m_PlayerPos->top + m_PlayerPos->height);

    if (m_Position.contains(feet)) {
        // Snap player to top of road
        m_PlayerPos->top = m_Position.top - m_PlayerPos->height;
        *m_PlayerGrounded = true;
    }
}
