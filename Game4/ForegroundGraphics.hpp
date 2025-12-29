#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"
#include "PlatformUpdate.hpp"

// Forward declaration so we can have a pointer to it
class PositionUpdate;
class LevelUpdate;

// Decorative objects drawn in front of gameplay elements.
class ForegroundGraphics : public Graphics
{
  private:
    sf::Sprite m_Sprite;
    std::string m_Path;

    // Pointer to the update component that contains the flower's position
    PositionUpdate *m_Update = nullptr;

    // A pointer to a platform update
    PlatformUpdate *m_PlatformUpdate = nullptr;

    // Level Logic
    LevelUpdate *m_LevelUpdate = nullptr;
    int m_VisibleLevel = 0; // 0 means always visible

  public:
    ForegroundGraphics(const std::string &texturePath);

    void assemble(std::shared_ptr<Update> genericUpdate) override;

    // Allow connecting to LevelUpdate
    void assemble(std::shared_ptr<LevelUpdate> levelUpdate);

    void draw(sf::RenderWindow &window) override;

    // Flower Transparency
    void setAlpha(sf::Uint8 alpha);

    // Set which level this appears in (0 = all)
    void setVisibleLevel(int level)
    {
        m_VisibleLevel = level;
    }
};
