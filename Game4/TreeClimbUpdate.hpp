#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Update.hpp"

class LevelUpdate;
class PlayerUpdate;

// -------------------------------------------------------------
// TreeClimbUpdate
//  - Gives the player a "climbable" tree trunk area.
//  - When the player overlaps the trunk and presses W/Up,
//    we tell PlayerUpdate to enter climbing mode.
// -------------------------------------------------------------
class TreeClimbUpdate : public Update
{
  private:
    // Axis-aligned box representing the climbable part of the trunk
    sf::FloatRect m_TrunkBounds;

    // Pointers into the player so we can read position + toggle climbing
    PlayerUpdate *m_Player = nullptr;
    sf::FloatRect *m_PlayerBounds = nullptr;

  public:
    // Constructor
    TreeClimbUpdate()
    {
        m_IsUpdate = true;
    }

    // Optional helper for Factory to configure the trunk area
    void setTrunkArea(float left, float top, float width, float height)
    {
        m_TrunkBounds = {left, top, width, height};
    }

    // Getter for trunk area pointer
    sf::FloatRect *getTrunkAreaPointer()
    {
        return &m_TrunkBounds;
    }

    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    void update(float dt) override;
};