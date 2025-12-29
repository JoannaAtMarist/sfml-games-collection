#include <cmath>
#include "ButterflyGraphics.hpp"
#include "Animator.hpp"
#include "TextureHolder.hpp"
#include "PlatformUpdate.hpp"

using namespace sf;

// Constructor
ButterflyGraphics::ButterflyGraphics(const std::string &texturePath)
    : m_Path(texturePath)
{
}

// Assemble with the associated Update component
void ButterflyGraphics::assemble(std::shared_ptr<Update> genericUpdate)
{
    // Attach to a platform
    m_PlatformUpdate =
        std::dynamic_pointer_cast<PlatformUpdate>(genericUpdate).get();

    // Load texture
    m_Texture = &TextureHolder::GetTexture(m_Path);
    m_Sprite.setTexture(*m_Texture);

    // Sprite sheet: 256x64 -> 8 frames in top row, each 32x32
    const int FRAME_W = 32;
    const int FRAME_H = 32;
    const int FRAME_COUNT = 8;
    const int FPS = 10;

    m_Animator =
        new Animator(0,                     // leftOffset
                     0,                     // topOffset
                     FRAME_COUNT,           // number of frames
                     FRAME_W * FRAME_COUNT, // row width in pixels (256)
                     FRAME_H,               // frame height
                     FPS                    // animation speed
        );

    // Make the butterfly a bit bigger
    m_Sprite.setScale(1.5f, 1.5f);
}

// Draw the butterfly
void ButterflyGraphics::draw(RenderWindow &window)
{
    if (!m_PlatformUpdate || !m_Texture)
        return;

    // Base position: centered over the platform, a bit above it
    Vector2f platPos = m_PlatformUpdate->getPosition();
    Vector2f platSize = m_PlatformUpdate->getSize();

    float baseX = platPos.x + platSize.x / 2.f;
    float baseY = platPos.y - 130.f; // above the platform

    // Hover motion using a sine wave
    float t = m_HoverClock.getElapsedTime().asSeconds();
    float hoverOffset =
        std::sin(t * 2.f * 3.1415926f * m_HoverSpeed) * m_HoverAmplitude;

    // Advance animation
    IntRect frame = *m_Animator->getCurrentFrame(false);
    m_Sprite.setTextureRect(frame);

    // Position with hover offset
    m_Sprite.setPosition(baseX, baseY + hoverOffset);

    window.draw(m_Sprite);
}
