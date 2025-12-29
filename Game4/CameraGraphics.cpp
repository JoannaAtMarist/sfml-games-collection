#include <iostream>
#include <string>
#include "CameraGraphics.hpp"
#include "CameraUpdate.hpp"
#include "LevelUpdate.hpp"
#include "TextureHolder.hpp"

using namespace sf;

class LevelUpdate;

// Define the level end X position
CameraGraphics::CameraGraphics()
{
    // Setup Camera View
    m_View.setSize(1280, 720);

    // Setup Font & Text
    if (!m_Font.loadFromFile("fonts/OzsWizard-CowardlyLion.ttf")) {
        std::cout << "Could not load font for Camera HUD\n";
    }
    m_DistanceText.setFont(m_Font);
    m_DistanceText.setCharacterSize(30);
    m_DistanceText.setFillColor(Color::White);
    m_DistanceText.setOutlineColor(Color::Black);
    m_DistanceText.setOutlineThickness(2.f);
    m_DistanceText.setPosition(20.f, 20.f); // Top-left corner of screen

    // Setup Finish Line (Visual Marker)
    // TODO: replace this
    m_FinishLine.setSize(Vector2f(20.f, 2000.f));     // Tall red strip
    m_FinishLine.setFillColor(Color(255, 0, 0, 150)); // Semi-transparent Red
    m_FinishLine.setPosition(LEVEL_END_X, -1000.f);   // Place it at the end
}

// Assemble with CameraUpdate to get position data
void CameraGraphics::assemble(std::shared_ptr<Update> genericUpdate)
{
    m_CameraUpdate = static_cast<CameraUpdate *>(genericUpdate.get());

    // 1. SKY (Far Background)
    m_BackgroundTexture =
        &TextureHolder::GetTexture("graphics/environment/sky.jpg");
    m_BackgroundSprite.setTexture(*m_BackgroundTexture);

    // Scale huge to cover everything
    m_BackgroundSprite.setScale(5.0f, 5.0f);

    // 2. MIDGROUND (Forest)
    m_MidgroundTexture =
        &TextureHolder::GetTexture("graphics/environment/middle.png");
    m_MidgroundTexture->setRepeated(true); // Loop it!
    m_MidgroundSprite.setTexture(*m_MidgroundTexture);

    // Initial scale
    m_MidgroundSprite.setScale(6.0f, 6.0f);
}

// Assemble with LevelUpdate to get level data
void CameraGraphics::assemble(std::shared_ptr<LevelUpdate> levelUpdate)
{
    m_LevelUpdate = levelUpdate.get();
}

// Draw the camera view
void CameraGraphics::draw(RenderWindow &window)
{
    if (!m_CameraUpdate)
        return;

    FloatRect *pos = m_CameraUpdate->getPositionPointer();
    Vector2u winSize = window.getSize();

    // Sync view size
    m_View.setSize(static_cast<float>(winSize.x),
                   static_cast<float>(winSize.y));
    m_View.setCenter(pos->left, pos->top - 100);

    // Zoom
    if (pos->width != 1.0f && pos->width > 0.0f) {
        m_View.zoom(pos->width);
    }

    // APPLY VIEW
    window.setView(m_View);

    Vector2f viewCenter = m_View.getCenter();
    Vector2f viewSize = m_View.getSize();

    //. 1. SKY LAYER (Very Slow Parallax)
    float bgX = viewCenter.x * 0.98f - (viewSize.x / 2);
    float bgY = viewCenter.y * 0.98f - (viewSize.y / 2);
    m_BackgroundSprite.setPosition(bgX, bgY - 300);
    window.draw(m_BackgroundSprite);

    //. 2. MIDGROUND LAYER (Forest) -> ONLY DRAW IN LEVEL 1
    bool showForest = true;
    if (m_LevelUpdate && m_LevelUpdate->getCurrentLevel() == 2) {
        showForest = false;
    }

    if (showForest) {
        float parallaxFactor = 0.2f;
        int textureOffset = static_cast<int>(viewCenter.x * parallaxFactor);

        // Calculate how much texture we need to cover the screen
        float scale = 6.0f;
        // Divide screen width by scale to get texture pixels needed
        int neededWidth = static_cast<int>(viewSize.x / scale) + 100;
        int texHeight = m_MidgroundTexture->getSize().y;

        // "Scroll" the texture by changing the IntRect left position
        m_MidgroundSprite.setTextureRect(
            sf::IntRect(textureOffset, 0, neededWidth, texHeight));

        // Anchor to bottom-left of view
        float screenLeft = viewCenter.x - (viewSize.x / 2.f);
        float screenBottom = viewCenter.y + (viewSize.y / 2.f);
        float spriteHeight = texHeight * scale;

        m_MidgroundSprite.setPosition(screenLeft,
                                      screenBottom - spriteHeight + 250.f);
        m_MidgroundSprite.setScale(scale, scale);

        window.draw(m_MidgroundSprite);
    }

    //. 3. GAMEPLAY OBJECTS (Finish Line, HUD)

    window.draw(m_FinishLine);

    // HUD (Screen Space)
    window.setView(window.getDefaultView());
    int distanceRemaining = static_cast<int>(LEVEL_END_X - pos->left);
    if (distanceRemaining < 0)
        distanceRemaining = 0;
    m_DistanceText.setString("Distance: " + std::to_string(distanceRemaining));
    window.draw(m_DistanceText);

    // Restore World View
    window.setView(m_View);
}