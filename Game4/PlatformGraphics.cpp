#include <cmath>
#include <SFML/Graphics.hpp>
#include "PlatformGraphics.hpp"
#include "TextureHolder.hpp"

using namespace sf;
using namespace std;

// Constructor
PlatformGraphics::PlatformGraphics() : Graphics()
{
}

// Assemble with the matching Update component
void PlatformGraphics::assemble(std::shared_ptr<Update> genericUpdate)
{
    m_Update = static_cast<PlatformUpdate *>(genericUpdate.get());

    // 1. Load the single tile texture
    m_Texture = &TextureHolder::GetTexture("graphics/handpaintedwall2y.png");

    // 2. Repeat this texture if the rect is bigger than the image
    // m_Texture->setRepeated(true);
}

// Draw the platform as tiled texture
void PlatformGraphics::draw(RenderWindow &window)
{
    if (!m_Update || !m_Texture)
        return;

    // Do not draw vanished platforms
    if (!m_Update->isActive())
        return;

    // Get the dynamic position and size from LevelUpdate logic
    Vector2f pos = m_Update->getPosition();
    Vector2f size = m_Update->getSize();

    // 1. Setup the Sprite
    Sprite s;
    s.setTexture(*m_Texture);

    // Figure out scaling to make tiles fit nicely
    float texW = static_cast<float>(m_Texture->getSize().x);
    float texH = static_cast<float>(m_Texture->getSize().y);

    // We want tiles to be square, so base both dimensions on height
    float tileH = size.y;
    float tileW = tileH; // Keep it square 1:1 aspect ratio

    float scaleX = tileW / texW;
    float scaleY = tileH / texH;

    s.setScale(scaleX, scaleY);

    // 2. Loop and Draw "Stamps"
    float currentX = 0;

    while (currentX < size.x) {

        // Calculate how much space is left
        float spaceRemaining = size.x - currentX;

        // Determine if we can fit a full tile, or need a partial one
        float widthToDraw = tileW;
        bool isPartial = false;

        if (spaceRemaining < tileW) {
            widthToDraw = spaceRemaining;
            isPartial = true;
        }

        // Set Position
        s.setPosition(pos.x + currentX, pos.y);

        // handle cropping if it's the last little piece
        if (isPartial) {
            // Calculate how many pixels of the TEXTURE we need
            // (Width / Scale = TexturePixels)
            int sourceWidth = static_cast<int>(widthToDraw / scaleX);
            s.setTextureRect(IntRect(0, 0, sourceWidth, (int)texH));
        } else {
            // Reset to full texture
            s.setTextureRect(IntRect(0, 0, (int)texW, (int)texH));
        }

        window.draw(s);

        currentX += tileW;
    }
}
