#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"

// ------------------------------------------------------------
// BackgroundGraphics
// Draws a full-screen background image (sky, clouds, etc.)
// Does not use an Update component.
// ------------------------------------------------------------
class BackgroundGraphics : public Graphics
{
  private:
    sf::Sprite m_Sprite;

  public:
    BackgroundGraphics();

    // assemble() is required by Graphics, but we don't use Update.
    void assemble(std::shared_ptr<Update> genericUpdate) override;

    // Draws the background sprite.
    void draw(sf::RenderWindow &window) override;
};
