#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Graphics.hpp"

class MenuUpdate;
class PlayerUpdate;

class MenuGraphics : public Graphics
{
  private:
    MenuUpdate *m_MenuUpdate = nullptr;

    sf::Font m_Font;
    sf::Text m_Text;

    // Helper to center text
    void centerText(sf::RenderWindow &window);

    // Health HUD
    PlayerUpdate *m_Player = nullptr;
    sf::Texture *m_UiTexture = nullptr;
    sf::Sprite m_HeartSprite;
    sf::IntRect m_HeartRect;

  public:
    MenuGraphics();
    void assemble(std::shared_ptr<Update> genericUpdate) override;
    void draw(sf::RenderWindow &window) override;
    void assemble(std::shared_ptr<PlayerUpdate> player);
};