#include "Animator.hpp"

// Constructor
Animator::Animator(int leftOffset, int topOffset, int frameCount,
                   int textureWidth, int textureHeight, int fps)
{
    m_LeftOffset = leftOffset;
    m_CurrentFrame = 0;
    m_FrameCount = frameCount == 0 ? 1 : frameCount;

    m_FrameWidth = static_cast<float>(textureWidth) / m_FrameCount;

    m_SourceRect.left = leftOffset;
    m_SourceRect.top = topOffset;
    m_SourceRect.width = m_FrameWidth;
    m_SourceRect.height = textureHeight;

    m_FPS = (fps == 0 ? 1 : fps);
    m_FramePeriod = 1000 / m_FPS;

    m_Clock.restart();
}

// Returns the IntRect for the correct animation frame.
sf::IntRect *Animator::getCurrentFrame(bool reversed)
{
    // Reversed adds 1 to the frame number
    // when drawing the texture reversed.
    // This works because reversed
    // (flipped horizontally) textures
    // are drawn pixels right to left

    if (m_Clock.getElapsedTime().asMilliseconds() > m_FramePeriod) {
        m_CurrentFrame++;

        if (m_CurrentFrame >= m_FrameCount + reversed)
            m_CurrentFrame = 0 + reversed;

        m_Clock.restart();
    }

    m_SourceRect.left = m_LeftOffset + m_CurrentFrame * m_FrameWidth;
    return &m_SourceRect;
}