#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"

class CameraUpdate;
class LevelUpdate;

class CameraGraphics : public Graphics
{
  private:
    CameraUpdate *m_CameraUpdate = nullptr;
    sf::View m_View;

    // Background assets
    sf::Sprite m_BackgroundSprite;
    sf::Texture *m_BackgroundTexture = nullptr;

    // Midground (Forest)
    sf::Sprite m_MidgroundSprite;
    sf::Texture *m_MidgroundTexture = nullptr;

    // UI Elements
    sf::Font m_Font;
    sf::Text m_DistanceText;

    // World Elements
    sf::RectangleShape m_FinishLine;
    const float LEVEL_END_X = 4000.f; // Must match LevelUpdate!

    LevelUpdate *m_LevelUpdate = nullptr;

  public:
    CameraGraphics();

    void assemble(std::shared_ptr<Update> genericUpdate) override;
    void assemble(std::shared_ptr<LevelUpdate> levelUpdate);

    void draw(sf::RenderWindow &window) override;
};