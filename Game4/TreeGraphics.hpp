#pragma once
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"
#include "Update.hpp"

class PositionUpdate;

// -------------------------------------------------------------
// TreeGraphics
//  - Draws a single tree sprite from a sprite sheet
//    using an sf::IntRect sub-rectangle.
//  - Uses PositionUpdate for world position.
// -------------------------------------------------------------
class TreeGraphics : public Graphics
{
  private:
    std::string m_Path;
    sf::IntRect m_Rect;

    sf::Sprite m_Sprite;
    PositionUpdate *m_Position = nullptr;

  public:
    TreeGraphics(const std::string &texturePath, const sf::IntRect &spriteRect);

    void assemble(std::shared_ptr<Update> genericUpdate) override;
    void draw(sf::RenderWindow &window) override;

    void setScale(float scale);
};
