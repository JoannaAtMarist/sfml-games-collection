#include "BackgroundGraphics.hpp"
#include "TextureHolder.hpp"

using namespace sf;

// Constructor
BackgroundGraphics::BackgroundGraphics() : Graphics()
{
}

// assemble() is required by Graphics, but we don't use Update.
void BackgroundGraphics::assemble(std::shared_ptr<Update> genericUpdate)
{
    // Load the background image using TextureHolder
    m_Sprite.setTexture(
        TextureHolder::GetTexture("graphics/environment/sky.jpg"));

    // Position it at the top-left of the screen
    m_Sprite.setPosition(0.f, 0.f);

    // Scale the background to fit the window size
    VideoMode mode = VideoMode::getDesktopMode();
    Vector2f winSize((float)mode.width, (float)mode.height);

    Vector2u texSize = m_Sprite.getTexture()->getSize();
    Vector2f scale(winSize.x / texSize.x, winSize.y / texSize.y);

    m_Sprite.setScale(scale);
}

// Draws the background sprite.
void BackgroundGraphics::draw(RenderWindow &window)
{
    window.draw(m_Sprite);
}
