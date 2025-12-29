#include <cmath>
#include <string>

#include "Bullet.hpp"
#include "TextureHolder.hpp"

using namespace sf;
using namespace std;

namespace {
    // Proton spritesheet info
    const char *PROTON_TEXTURE_PATH = "graphics/proton.png";

    // Sheet is 576x192, grid 64x64, 9 frames left->right on row 0
    const int PROTON_FRAME_WIDTH = 190;
    const int PROTON_FRAME_HEIGHT = 64;
    const int PROTON_FRAMES = 9;

    // Scale to adjust beam bigger/smaller
    const float PROTON_SCALE = 1.0f; // 1.0 = original size
} // namespace

// Constructor
Bullet::Bullet()
{
    // Load the spritesheet once
    Texture &tex = TextureHolder::GetTexture(PROTON_TEXTURE_PATH);
    m_BulletSprite.setTexture(tex);

    // Start on frame 0, row 0
    m_BulletSprite.setTextureRect(
        IntRect(0, 0, PROTON_FRAME_WIDTH, PROTON_FRAME_HEIGHT));

    // Optional scaling
    m_BulletSprite.setScale(PROTON_SCALE, PROTON_SCALE);

    // Center origin so rotation looks good
    FloatRect bounds = m_BulletSprite.getLocalBounds();
    m_BulletSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    // Frame timing (adjustable)
    m_FrameTime = 0.055f;
}

// Launch a new bullet
void Bullet::shoot(float startX, float startY, float targetX, float targetY)
{
    m_InFlight = true;

    // Position
    m_Position.x = startX;
    m_Position.y = startY;
    m_StartPosition = m_Position;
    m_DistanceTravelled = 0.0f;

    // Reset animation
    m_CurrentFrame = 0;
    m_TimeSinceFrame = 0.0f;
    m_BulletSprite.setTextureRect(
        IntRect(0, 0, PROTON_FRAME_WIDTH, PROTON_FRAME_HEIGHT));

    // Calculate gradient of path
    float dy = (startY - targetY);
    if (dy > -0.0001f && dy < 0.0001f)
        dy = 0.0001f; // avoid divide-by-zero
    float gradient = (startX - targetX) / dy;

    if (gradient < 0)
        gradient *= -1;

    // Ratio X/Y based on speed
    float ratioXY = m_BulletSpeed / (1 + gradient);
    m_BulletDistanceY = ratioXY;
    m_BulletDistanceX = ratioXY * gradient;

    // Flip direction as needed
    if (targetX < startX)
        m_BulletDistanceX *= -1;
    if (targetY < startY)
        m_BulletDistanceY *= -1;

    // push the spawn point out in front of the player
    {
        // Direction unit vector
        float len = std::sqrt(m_BulletDistanceX * m_BulletDistanceX +
                              m_BulletDistanceY * m_BulletDistanceY);
        if (len > 0.f) {
            float ux = m_BulletDistanceX / len;
            float uy = m_BulletDistanceY / len;

            // How far in front of the player the beam starts
            const float muzzleOffset = 50.f; // tweak: 30–60 ish

            m_Position.x += ux * muzzleOffset;
            m_Position.y += uy * muzzleOffset;
        }
    }

    // Now this is the true start of the beam
    m_StartPosition = m_Position;

    // Rotate sprite to match direction
    float angleRad = std::atan2(m_BulletDistanceY, m_BulletDistanceX);
    float angleDeg = angleRad * 180.f / 3.14159265f;
    m_BulletSprite.setRotation(angleDeg);

    // World range limits (backup to distance-based kill)
    float range = 1000.f;
    m_MinX = startX - range;
    m_MaxX = startX + range;
    m_MinY = startY - range;
    m_MaxY = startY + range;

    // Place sprite
    m_BulletSprite.setPosition(m_Position);
}

// Update movement + animation
void Bullet::update(float elapsedTime)
{
    if (!m_InFlight)
        return;

    // Move along trajectory
    float dx = m_BulletDistanceX * elapsedTime;
    float dy = m_BulletDistanceY * elapsedTime;

    m_Position.x += dx;
    m_Position.y += dy;
    m_BulletSprite.setPosition(m_Position);

    // Track distance travelled (for consistent lifetime)
    m_DistanceTravelled += std::sqrt(dx * dx + dy * dy);

    // Animate frames on the spritesheet (frames stacked vertically)
    m_TimeSinceFrame += elapsedTime;
    if (m_TimeSinceFrame >= m_FrameTime) {
        m_TimeSinceFrame -= m_FrameTime;
        m_CurrentFrame = (m_CurrentFrame + 1) % PROTON_FRAMES;

        int left = 0; // single column
        int top = m_CurrentFrame * PROTON_FRAME_HEIGHT;

        m_BulletSprite.setTextureRect(
            IntRect(left, top, PROTON_FRAME_WIDTH, PROTON_FRAME_HEIGHT));
    }

    // Kill bullet if it goes too far (distance based)
    if (m_DistanceTravelled > m_MaxDistance) {
        m_InFlight = false;
        return;
    }

    // Extra safety: world bounds check
    if (m_Position.x < m_MinX || m_Position.x > m_MaxX ||
        m_Position.y < m_MinY || m_Position.y > m_MaxY) {
        m_InFlight = false;
    }
}

// Stop the bullet
void Bullet::stop()
{
    m_InFlight = false;
}

// Is the bullet currently flying?
bool Bullet::isInFlight()
{
    return m_InFlight;
}

// Collision box in world space
FloatRect Bullet::getPosition()
{
    auto bounds = m_BulletSprite.getGlobalBounds();

    // Shrink collision a bit so it hugs the beam
    const float shrinkX = bounds.width * 0.5f;
    const float shrinkY = bounds.height * 0.5f;

    bounds.left += shrinkX / 2.f;
    bounds.top += shrinkY / 2.f;
    bounds.width -= shrinkX;
    bounds.height -= shrinkY;

    return bounds;
}

// Sprite for drawing
Sprite &Bullet::getSprite()
{
    return m_BulletSprite;
}
