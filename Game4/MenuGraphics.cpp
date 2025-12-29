#include <iostream>
#include "MenuGraphics.hpp"
#include "MenuUpdate.hpp"
#include "PlayerUpdate.hpp"
#include "TextureHolder.hpp"

using namespace sf;

// Constructor
MenuGraphics::MenuGraphics()
{
    // Try to load a font. If you don't have this, pick any .ttf file you have.
    // If it fails, nothing will draw, but it won't crash.
    if (!m_Font.loadFromFile("fonts/OzsWizard-CowardlyLion.ttf")) {
        // Fallback or error logging
        std::cout << "Could not load font for Menu\n";
    }
    m_Text.setFont(m_Font);
    m_Text.setCharacterSize(80);
    m_Text.setFillColor(Color::White);
    m_Text.setOutlineColor(Color::Black);
    m_Text.setOutlineThickness(3);
}

// Assemble with MenuUpdate
void MenuGraphics::assemble(std::shared_ptr<Update> genericUpdate)
{
    m_MenuUpdate = static_cast<MenuUpdate *>(genericUpdate.get());
}

// Assemble with Player for Health HUD
void MenuGraphics::assemble(std::shared_ptr<PlayerUpdate> player)
{
    m_Player = player.get();

    // Load the UI sheet once
    m_UiTexture = &TextureHolder::GetTexture("graphics/ui.png");
    m_HeartSprite.setTexture(*m_UiTexture);

    m_HeartRect = sf::IntRect(8, 136, 8, 8);

    m_HeartSprite.setTextureRect(m_HeartRect);
    m_HeartSprite.setScale(2.f, 2.f); // make it readable
}

// Helper to center text
void MenuGraphics::centerText(sf::RenderWindow &window)
{
    sf::FloatRect textRect = m_Text.getLocalBounds();
    m_Text.setOrigin(textRect.left + textRect.width / 2.0f,
                     textRect.top + textRect.height / 2.0f);
    m_Text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
}

// Draw the menu graphics
void MenuGraphics::draw(sf::RenderWindow &window)
{
    if (!m_MenuUpdate)
        return;

    // Save whatever view was active when we were called
    sf::View previousView = window.getView();

    // Get status from the Update component
    bool *paused = m_MenuUpdate->getIsPausedPointer();
    bool *gameOver = m_MenuUpdate->getGameOverPointer();
    bool *gameWon = m_MenuUpdate->getGameWonPointer(); // NEW
    sf::FloatRect *pos = m_MenuUpdate->getPositionPointer();

    // Get the first load pointer
    bool *firstLoad = m_MenuUpdate->getFirstLoadPointer();

    // Switch to UI view (so HUD is screen-space, not world-space)
    sf::View defaultView = window.getDefaultView();
    window.setView(defaultView);

    //. 1) Text
    if (pos->left > -100) {

        // 1. VICTORY
        if (gameWon && *gameWon) {
            m_Text.setString("VICTORY!");
            m_Text.setFillColor(sf::Color::Green);
            m_Text.setCharacterSize(80);
        }
        // 2. FIRST LOAD (Start Screen)
        else if (*firstLoad) {
            m_Text.setString("PRESS ENTER");
            m_Text.setFillColor(sf::Color::Yellow);
            m_Text.setCharacterSize(80);
        }
        // 3. GAME OVER (Death)
        else if (*gameOver) {
            // Split into two lines so it fits on screen
            m_Text.setString("GAME OVER...\nPress Enter to try again!");
            m_Text.setFillColor(sf::Color::Red);

            // Reduce size slightly so the long text looks better
            m_Text.setCharacterSize(60);
        }
        // 4. PAUSED
        else if (*paused) {
            m_Text.setString("PAUSED");
            m_Text.setFillColor(sf::Color::Yellow);
            m_Text.setCharacterSize(80);
        }

        // Center the text
        sf::FloatRect textRect = m_Text.getLocalBounds();
        m_Text.setOrigin(textRect.left + textRect.width / 2.0f,
                         textRect.top + textRect.height / 2.0f);
        m_Text.setPosition(defaultView.getCenter());

        window.draw(m_Text);
    }

    //. 2) Health HUD: always draw
    if (m_Player && m_UiTexture) {
        int maxHp = m_Player->getMaxHealth();
        int hp = m_Player->getHealth();

        float startX = 20.f;
        float startY = 70.f; // under "Distance:"
        float spacing = m_HeartRect.width * m_HeartSprite.getScale().x + 4.f;

        for (int i = 0; i < maxHp; ++i) {
            if (i >= hp)
                break;

            m_HeartSprite.setPosition(startX + i * spacing, startY);
            m_HeartSprite.setTextureRect(m_HeartRect);
            window.draw(m_HeartSprite);
        }
    }

    // put the view back how we found it
    window.setView(previousView);
}
