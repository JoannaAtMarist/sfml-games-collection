#pragma once
#include <SFML/Graphics.hpp>

// -------------------------------------------------------------
// Animator
//  - Handles frame-based animation.
//  - Computes the correct texture rectangle for the current frame.
// -------------------------------------------------------------
class Animator
{
  private:
    sf::IntRect m_SourceRect; // Current rectangle of the texture to draw
    int m_LeftOffset;         // Left offset of first frame in sprite sheet
    int m_FrameCount;         // Total number of animation frames
    int m_CurrentFrame;       // Current frame of animation
    int m_FramePeriod;        // Milliseconds between frame changes
    int m_FrameWidth;         // Width of each frame in the texture
    int m_FPS = 12;           // Animation frames per second
    sf::Clock m_Clock;        // Timing clock

  public:
    Animator(int leftOffset, int topOffset, int frameCount, int textureWidth,
             int textureHeight, int fps);

    // Returns the IntRect for the correct animation frame.
    sf::IntRect *getCurrentFrame(bool reversed);
};