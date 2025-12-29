#include "BeeProjectileGraphics.hpp"
#include "BeeProjectileUpdate.hpp"
#include "Animator.hpp"
#include "TextureHolder.hpp"

using namespace sf;

// Constructor
BeeProjectileGraphics::BeeProjectileGraphics() = default;

// Link to the Update component and set up texture & animator
void BeeProjectileGraphics::assemble(std::shared_ptr<Update> update)
{
    m_Update = dynamic_cast<BeeProjectileUpdate *>(update.get());
    if (!m_Update)
        return;

    m_Texture = &TextureHolder::GetTexture("graphics/bee.png");
    m_Sprite.setTexture(*m_Texture);

    const int FRAME_W = 32;
    const int FRAME_H = 32;
    const int FRAME_COUNT = 4;
    const int FPS = 10;

    // Left-facing
    m_Animator = new Animator(0,  // leftOffset
                              32, // topOffset (row 1 = left)
                              FRAME_COUNT, FRAME_W * FRAME_COUNT, FRAME_H, FPS);

    m_Sprite.setScale(2.f, 2.f); // slightly bigger
}

// Draw the bee projectile
void BeeProjectileGraphics::draw(RenderWindow &window)
{
    if (!m_Update || !m_Texture)
        return;

    // Advance animation
    IntRect frame = *m_Animator->getCurrentFrame(false);
    m_Sprite.setTextureRect(frame);

    const FloatRect &b = m_Update->getBounds();
    m_Sprite.setPosition(b.left, b.top);

    window.draw(m_Sprite);
}
