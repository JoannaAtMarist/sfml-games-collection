#include "ForegroundGraphics.hpp"
#include "LevelUpdate.hpp"
#include "PositionUpdate.hpp"
#include "PlatformUpdate.hpp"
#include "TextureHolder.hpp"

using namespace sf;

// Constructor
ForegroundGraphics::ForegroundGraphics(const std::string &texturePath)
    : m_Path(texturePath)
{
}

// Connect to Update component
void ForegroundGraphics::assemble(std::shared_ptr<Update> genericUpdate)
{
    // Save PositionUpdate pointer
    m_Update = static_cast<PositionUpdate *>(genericUpdate.get());

    m_Sprite.setTexture(TextureHolder::GetTexture(m_Path));

    // Bottom-center origin is perfect for sitting on things
    FloatRect bounds = m_Sprite.getLocalBounds();
    m_Sprite.setOrigin(bounds.width / 2.f, bounds.height);
    m_Sprite.setScale(0.4f, 0.4f);

    // TRY TO CAST TO PLATFORM UPDATE FIRST
    m_PlatformUpdate = dynamic_cast<PlatformUpdate *>(genericUpdate.get());

    // IF NOT A PLATFORM, TRY POSITION UPDATE (for the manually placed ones)
    if (!m_PlatformUpdate) {
        m_Update = dynamic_cast<PositionUpdate *>(genericUpdate.get());
    }

    // Scale flowers down
    m_Sprite.setScale(0.4f, 0.4f);
}

// Connect to LevelUpdate
void ForegroundGraphics::assemble(std::shared_ptr<LevelUpdate> levelUpdate)
{
    m_LevelUpdate = levelUpdate.get();
}

// Draw the flower
void ForegroundGraphics::draw(RenderWindow &window)
{
    // Check Level Visibility
    if (m_LevelUpdate && m_VisibleLevel > 0) {
        if (m_LevelUpdate->getCurrentLevel() != m_VisibleLevel) {
            return; // Wrong level, don't draw!
        }
    }

    // CASE A: Attached to a Moving Platform
    if (m_PlatformUpdate) {
        Vector2f platPos = m_PlatformUpdate->getPosition();
        Vector2f platSize = m_PlatformUpdate->getSize();

        // Horizontal: Center of the platform
        float x = platPos.x + (platSize.x / 2.f);
        // Vertical: Top of the platform
        float y = platPos.y + 20.f;

        m_Sprite.setPosition(x, y);
    }
    // CASE B: Static Position
    else if (m_Update) {
        Vector2f pos = m_Update->getPosition();
        m_Sprite.setPosition(pos);
    }

    window.draw(m_Sprite);
}

// Set flower transparency
void ForegroundGraphics::setAlpha(Uint8 alpha)
{
    Color c = m_Sprite.getColor();
    c.a = alpha;
    m_Sprite.setColor(c);
}
