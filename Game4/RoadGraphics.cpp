#include "RoadGraphics.hpp"
#include "TextureHolder.hpp"

// Constructor to create multiple tiled sprites for the road
RoadGraphics::RoadGraphics(const std::string &texturePath, int numTiles,
                           float tileW, float tileH, float yPos)
{
    auto &tex = TextureHolder::GetTexture(texturePath);

    // Get the original texture size
    float texW = static_cast<float>(tex.getSize().x);
    float texH = static_cast<float>(tex.getSize().y);

    // Scale factors to fit the tile size
    float scaleX = tileW / texW;
    float scaleY = tileH / texH;

    for (int i = 0; i < numTiles; i++) {
        sf::Sprite s;
        s.setTexture(tex);

        // Apply the scale so each copy is tileW by tileH
        s.setScale(scaleX, scaleY);

        // Position each tile right next to the previous one
        s.setPosition(i * tileW, yPos);

        m_Tiles.push_back(s);
    }
}

// Draw all the road tiles to the window
void RoadGraphics::draw(sf::RenderWindow &window)
{
    for (auto &s : m_Tiles)
        window.draw(s);
}
