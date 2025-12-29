
#include <SFML/Graphics.hpp>
#include "Animator.hpp"
#include "PlayerGraphics.hpp"
#include "PlayerUpdate.hpp"
#include "TextureHolder.hpp"

using namespace sf;
using namespace std;

// Assemble the PlayerGraphics component
void PlayerGraphics::assemble(RenderWindow &window,
                              shared_ptr<Update> genericUpdate)
{
    // Convert update -> PlayerUpdate
    m_PlayerUpdate = static_pointer_cast<PlayerUpdate>(genericUpdate).get();
    m_Position = m_PlayerUpdate->getPositionPointer();

    // 1. Load All Textures
    m_WalkTexture =
        &TextureHolder::GetTexture("graphics/player/BillinaWalking.png");
    m_IdleTexture =
        &TextureHolder::GetTexture("graphics/player/BillinaIdle.png");
    m_DuckTexture =
        &TextureHolder::GetTexture("graphics/player/BillinaSitting.png");
    m_HurtTexture =
        &TextureHolder::GetTexture("graphics/player/BillinaHurt.png");

    // 2. Setup Animators
    const int FRAME_SIZE = 16;
    const int SCALE = 3;

    // Walk: 4 Frames
    m_WalkAnimator = new Animator(0, 0, 4, 16 * 4, 16, 12); // 12 FPS

    // Idle: 2 Frames (Bobs head)
    m_IdleAnimator = new Animator(0, 0, 2, 16 * 2, 16, 4);

    // Duck: 4 Frames (Sits down)
    m_DuckAnimator = new Animator(0, 0, 4, 16 * 4, 16, 8);

    // Hurt: 8 Frames (Flashes red)
    m_HurtAnimator = new Animator(0, 0, 8, 16 * 8, 16, 12);

    // Initial Setup
    m_Sprite.setTexture(*m_IdleTexture);
    m_Sprite.setScale(SCALE, SCALE);
}

// Update the sprite based on player state
void PlayerGraphics::updateSprite()
{
    // Get States
    bool isMovingRight = m_PlayerUpdate->m_RightIsHeldDown;
    bool isMovingLeft = m_PlayerUpdate->m_LeftIsHeldDown;
    bool isMoving = (isMovingLeft || isMovingRight);

    bool isDucking = m_PlayerUpdate->isDucking();
    bool isHurt = m_PlayerUpdate->isInvincible();

    // Default Facing Direction (Right)
    float scaleX = 3.f;
    float scaleY = 3.f;
    int originX = 0;

    // SQUISH LOGIC
    if (isDucking) {
        // Make her wider and shorter
        scaleX = 3.5f;
        scaleY = 2.0f;
    }

    // Flip Logic
    if (isMovingLeft) {
        scaleX = -3.f;
        originX = 16; // Shift origin to right edge so it flips in place
    }

    // ANIMATION STATE MACHINE

    // PRIORITY 1: HURT
    if (isHurt) {
        m_Sprite.setTexture(*m_HurtTexture);
        m_Sprite.setTextureRect(*m_HurtAnimator->getCurrentFrame(false));
    }
    // PRIORITY 2: DUCKING
    else if (isDucking) {
        m_Sprite.setTexture(*m_DuckTexture);
        m_Sprite.setTextureRect(*m_DuckAnimator->getCurrentFrame(false));
    }
    // PRIORITY 3: MOVING
    else if (isMoving) {
        m_Sprite.setTexture(*m_WalkTexture);
        m_Sprite.setTextureRect(*m_WalkAnimator->getCurrentFrame(false));
    }
    // PRIORITY 4: IDLE
    else {
        m_Sprite.setTexture(*m_IdleTexture);
        m_Sprite.setTextureRect(*m_IdleAnimator->getCurrentFrame(false));
    }

    // POSITIONING LOGIC

    // 1. Find the bottom of the logical hitbox (The feet)
    float feetY = m_Position->top + m_Position->height;

    // 2. Calculate how tall the sprite visually is right now
    // (16 pixels * current scale)
    float currentSpriteHeight = 16.f * std::abs(scaleY);

    // 3. Draw the sprite so its bottom aligns with the feet
    // (Feet Y - Sprite Height = Top Y)
    float drawY = feetY - currentSpriteHeight;

    m_Sprite.setPosition(m_Position->left, drawY);
    m_Sprite.setScale(scaleX, scaleY);
    m_Sprite.setOrigin(originX, 0);
}

// Draw the player sprite
void PlayerGraphics::draw(RenderWindow &window)
{
    updateSprite();
    window.draw(m_Sprite);
}