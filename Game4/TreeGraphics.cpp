#include "TreeGraphics.hpp"
#include "PositionUpdate.hpp"
#include "TextureHolder.hpp"

using namespace sf;

// Constructor
TreeGraphics::TreeGraphics(const std::string &texturePath,
                           const IntRect &spriteRect)
    : m_Path(texturePath), m_Rect(spriteRect)
{
}

// Assemble with the corresponding Update component
void TreeGraphics::assemble(std::shared_ptr<Update> genericUpdate)
{
    // We expect a PositionUpdate
    m_Position = dynamic_cast<PositionUpdate *>(genericUpdate.get());
    if (!m_Position)
        return;

    // Load the sheet and apply the sub-rect
    Texture &tex = TextureHolder::GetTexture(m_Path);
    m_Sprite.setTexture(tex);
    m_Sprite.setTextureRect(m_Rect);

    // Default scale if none set (will be overridden by Factory)
    // m_Sprite.setScale(1.f, 1.f);
}

// Draw the tree at its current position
void TreeGraphics::draw(RenderWindow &window)
{
    if (!m_Position)
        return;

    Vector2f pos = m_Position->getPosition();
    m_Sprite.setPosition(pos);

    window.draw(m_Sprite);
}

// Set the scale of the tree sprite
void TreeGraphics::setScale(float scale)
{
    m_Sprite.setScale(scale, scale);
}