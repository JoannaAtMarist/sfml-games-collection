#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"
#include "PlatformUpdate.hpp"

class PlatformGraphics : public Graphics
{
  private:
    PlatformUpdate *m_Update = nullptr;
    sf::Texture *m_Texture = nullptr;

    // Just 4 vertices (1 Quad) needed for a repeating tile
    sf::VertexArray m_Vertices;

  public:
    PlatformGraphics();

    void assemble(std::shared_ptr<Update> genericUpdate) override;
    void draw(sf::RenderWindow &window) override;
};
