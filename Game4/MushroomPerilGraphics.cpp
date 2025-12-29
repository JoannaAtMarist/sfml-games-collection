#include "MushroomPerilGraphics.hpp"
#include "MushroomPerilUpdate.hpp"
#include "TextureHolder.hpp"

using namespace sf;

// Constructor to set the texture path
MushroomPerilGraphics::MushroomPerilGraphics(const std::string &texturePath)
    : m_Path(texturePath)
{
}

// Link to the update component and get the texture
void MushroomPerilGraphics::assemble(std::shared_ptr<Update> update)
{
    m_Update = dynamic_cast<MushroomPerilUpdate *>(update.get());
    if (!m_Update)
        return;

    m_Texture = &TextureHolder::GetTexture(m_Path);
    m_Sprite.setTexture(*m_Texture);

    // Scale down if the PNG is large
    m_Sprite.setScale(0.4f, 0.4f);
}

// Draw the sprite at the position from the update component
void MushroomPerilGraphics::draw(RenderWindow &window)
{
    if (!m_Update || !m_Texture)
        return;

    const Vector2f &pos = m_Update->getPosition();
    m_Sprite.setPosition(pos.x, pos.y);

    window.draw(m_Sprite);
}
