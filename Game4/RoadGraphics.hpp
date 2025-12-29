#pragma once
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"

class RoadGraphics : public Graphics
{
  private:
    std::vector<sf::Sprite> m_Tiles;

  public:
    RoadGraphics(const std::string &texturePath, int numTiles, float tileW,
                 float tileH, float yPos);

    void draw(sf::RenderWindow &window) override;
};
