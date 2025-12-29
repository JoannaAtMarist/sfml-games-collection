#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"
#include "PlatformUpdate.hpp"

class Animator;
class PlatformUpdate;

// Animated butterfly hovering above a platform
class ButterflyGraphics : public Graphics
{
  private:
    std::string m_Path;

    sf::Sprite m_Sprite;
    sf::Texture *m_Texture = nullptr;

    PlatformUpdate *m_PlatformUpdate = nullptr;

    Animator *m_Animator = nullptr;

    // Hover motion
    sf::Clock m_HoverClock;
    float m_HoverAmplitude = 10.f; // pixels up/down
    float m_HoverSpeed = 1.5f;     // cycles per second

  public:
    explicit ButterflyGraphics(const std::string &texturePath);

    void assemble(std::shared_ptr<Update> genericUpdate) override;
    void draw(sf::RenderWindow &window) override;
};
