#include "TreeClimbUpdate.hpp"
#include "LevelUpdate.hpp"
#include "PlayerUpdate.hpp"

using namespace sf;
using namespace std;

// Assemble: get pointer to player and its position
void TreeClimbUpdate::assemble(shared_ptr<LevelUpdate> /*levelUpdate*/,
                               shared_ptr<PlayerUpdate> playerUpdate)
{
    m_Player = playerUpdate.get();

    if (m_Player) {
        // Use the same collider that everything else uses
        m_PlayerBounds = m_Player->getPositionPointer();
    }
}

// Update: check for overlap and manage climbing state
void TreeClimbUpdate::update(float /*dt*/)
{
    if (!m_Player || !m_PlayerBounds)
        return;

    // Check overlap between player and tree trunk
    bool overlapping = m_TrunkBounds.intersects(*m_PlayerBounds);

    // Keys that should initiate / maintain a climb
    bool climbKeyHeld = Keyboard::isKeyPressed(Keyboard::W) ||
                        Keyboard::isKeyPressed(Keyboard::Up) ||
                        Keyboard::isKeyPressed(Keyboard::S) ||
                        Keyboard::isKeyPressed(Keyboard::Down);

    // Start or continue climbing when overlapping + holding a climb key
    if (overlapping && climbKeyHeld) {
        float trunkCenterX = m_TrunkBounds.left + m_TrunkBounds.width / 2.f;

        // Turn on climb mode in the player
        m_Player->m_IsClimbing = true;
        m_Player->m_ClimbAnchorX = trunkCenterX;

        // Snap player horizontally to the trunk center so they "stick" to it
        m_PlayerBounds->left = trunkCenterX - (m_PlayerBounds->width / 2.f);
    }
    // Stop climbing if they are no longer touching the trunk
    else if (!overlapping) {
        m_Player->m_IsClimbing = false;
    }

    // Optional: if they press jump while climbing, let go of the tree
    if (m_Player->m_IsClimbing && Keyboard::isKeyPressed(Keyboard::Space)) {
        m_Player->m_IsClimbing = false;
    }
}