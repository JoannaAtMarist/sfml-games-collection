#include "MushroomPerilUpdate.hpp"
#include "LevelUpdate.hpp"

using namespace sf;

// Constructor
MushroomPerilUpdate::MushroomPerilUpdate() = default;

// Assemble function to connect to Level and Player
void MushroomPerilUpdate::assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                                   std::shared_ptr<PlayerUpdate> playerUpdate)
{
    // Update assemble to actually STORE the level update
    m_LevelUpdate = levelUpdate.get();

    if (playerUpdate) {
        m_PlayerRect = playerUpdate->getPositionPointer();
        m_Player = playerUpdate.get();
    }
}

// Set position and align hurtbox
void MushroomPerilUpdate::setPosition(float x, float y)
{
    m_Position.x = x;
    m_Position.y = y;

    //  Align the hurtbox to match the visual sprite (Bottom-Center Origin)
    // 1. Center horizontally (X is the center, so subtract half width)
    m_Area.left = x - (m_Area.width / 2.f);
    // 2. Sit on top of the road (Y is the ground, so subtract height to go up)
    m_Area.top = y - m_Area.height;
}

// Set size and re-align hurtbox
void MushroomPerilUpdate::setAreaSize(float width, float height)
{
    m_Area.width = width;
    m_Area.height = height;

    // Re-calculate the position logic when size changes.

    m_Area.left = m_Position.x - (m_Area.width / 2.f);
    m_Area.top = m_Position.y - m_Area.height;
}

// Update function to check for player collision
void MushroomPerilUpdate::update(float dt)
{
    if (!m_PlayerRect || !m_Player)
        return;

    // LEVEL CHECK: If we are in the wrong level, do nothing
    if (m_LevelUpdate && m_ActiveLevel != 0) {
        if (m_LevelUpdate->getCurrentLevel() != m_ActiveLevel) {
            return;
        }
    }

    // AoE: if Billina overlaps the mushroom's damage area, hurt her.
    if (m_Area.intersects(*m_PlayerRect)) {
        m_Player->takeDamage(m_Damage); // Use variable damage
    }
}
