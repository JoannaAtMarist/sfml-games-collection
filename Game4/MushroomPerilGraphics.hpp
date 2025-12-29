#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"

class MushroomPerilUpdate;

class MushroomPerilGraphics : public Graphics
{
  private:
    std::string m_Path;
    MushroomPerilUpdate *m_Update = nullptr;

    sf::Texture *m_Texture = nullptr;
    sf::Sprite m_Sprite;

  public:
    explicit MushroomPerilGraphics(const std::string &texturePath);

    void assemble(std::shared_ptr<Update> update) override;
    void draw(sf::RenderWindow &window) override;
};
