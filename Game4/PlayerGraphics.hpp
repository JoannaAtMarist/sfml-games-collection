#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Animator.hpp"
#include "Graphics.hpp"

class PlayerUpdate;

class PlayerGraphics : public Graphics
{
  private:
    PlayerUpdate *m_PlayerUpdate = nullptr;
    sf::FloatRect *m_Position = nullptr;

    sf::Sprite m_Sprite;

    // TEXTURES
    sf::Texture *m_WalkTexture = nullptr;
    sf::Texture *m_IdleTexture = nullptr;
    sf::Texture *m_DuckTexture = nullptr;
    sf::Texture *m_HurtTexture = nullptr;

    // ANIMATORS
    Animator *m_WalkAnimator = nullptr; // 4 frames
    Animator *m_IdleAnimator = nullptr; // 2 frames
    Animator *m_DuckAnimator = nullptr; // 4 frames
    Animator *m_HurtAnimator = nullptr; // 8 frames

  public:
    PlayerGraphics()
    {
        m_IsGraphics = true;
    }

    void assemble(sf::RenderWindow &window,
                  std::shared_ptr<Update> genericUpdate);

    void updateSprite(); // internal helper
    void draw(sf::RenderWindow &window) override;
};