#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"

class BeeProjectileUpdate;
class Animator;

class BeeProjectileGraphics : public Graphics
{
  private:
    BeeProjectileUpdate *m_Update = nullptr;

    sf::Texture *m_Texture = nullptr;
    sf::Sprite m_Sprite;
    Animator *m_Animator = nullptr;

  public:
    BeeProjectileGraphics();

    void assemble(std::shared_ptr<Update> update) override;
    void draw(sf::RenderWindow &window) override;
};
