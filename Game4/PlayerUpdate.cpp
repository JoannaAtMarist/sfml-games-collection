
#include "LevelUpdate.hpp"
#include "PlayerUpdate.hpp"
#include "SoundEngine.hpp"

using namespace sf;
using namespace std;

// Position Accessor
FloatRect *PlayerUpdate::getPositionPointer()
{
    return &m_Position;
}

// Grounded State Accessor
bool *PlayerUpdate::getGroundedPointer()
{
    return &m_IsGrounded;
}

// Input Receiver Accessor
InputReceiver *PlayerUpdate::getInputReceiver()
{
    return &m_InputReceiver;
}

// Assembly with Level
void PlayerUpdate::assemble(shared_ptr<LevelUpdate> levelUpdate,
                            shared_ptr<PlayerUpdate> playerUpdate)
{
    // SoundEngine::SoundEngine();

    m_Position.left = 200; // starting X
    m_Position.top = 10;   // starting Y
    m_Position.width = PLAYER_WIDTH;
    m_Position.height = PLAYER_HEIGHT;

    m_IsPaused = levelUpdate ? levelUpdate->getIsPausedPointer() : nullptr;
}

// Flight Powerup
void PlayerUpdate::grantFlight(float seconds)
{
    m_FlightDuration = seconds;
    m_FlightActive = true;
    m_FlightClock.restart();

    // Optional: reset jump state so flight feels clean
    m_InJump = false;
    m_JumpsRemaining = MAX_JUMPS;
}

// Health and Damage
void PlayerUpdate::takeDamage(int amount)
{
    // If paused or already invincible, ignore
    if ((m_IsPaused && *m_IsPaused) || m_IsInvincible)
        return;

    m_Health -= amount;
    if (m_Health < 0)
        m_Health = 0;

    // Start invincibility window so we don't take damage every frame
    m_IsInvincible = true;
    m_InvincibleClock.restart();

    std::cout << "Player HP is now: " << m_Health << "\n";
}

// Input Handling
void PlayerUpdate::handleInput()
{
    //. Check if the Sprint Key (Shift or W) is held
    m_SprintIsHeldDown = Keyboard::isKeyPressed(Keyboard::LShift) ||
                         Keyboard::isKeyPressed(Keyboard::W);

    //. DUCK & DROP LOGIC
    // Check if "Down" keys are held
    bool downHeld = Keyboard::isKeyPressed(Keyboard::S) ||
                    Keyboard::isKeyPressed(Keyboard::Down);
    bool spacePressed = Keyboard::isKeyPressed(Keyboard::Space);

    // Reset frame flags
    m_IsDucking = false;
    m_IsDropping = false;

    if (downHeld && !m_IsClimbing) {
        if (spacePressed) {
            // ACTION 2: DROP THROUGH
            m_IsDropping = true;
        } else {
            // ACTION 3: DUCK
            m_IsDucking = true;
        }
    }

    for (const Event &event : m_InputReceiver.getEvents()) {
        if (event.type == Event::KeyPressed) {
            if (event.key.code == Keyboard::D) {
                m_RightIsHeldDown = true;
            }
            if (event.key.code == Keyboard::A) {
                m_LeftIsHeldDown = true;
            }

            //. JUMP LOGIC (Triggers on Press)
            // IMPORTANT: Prevent normal jump if we are dropping!
            if (event.type == Event::KeyPressed &&
                event.key.code == Keyboard::Space) {
                if (m_IsDropping) {
                    // Do nothing (don't jump)
                } else if (m_JumpsRemaining > 0 && m_JumpKeyReleased) {
                    // Sound
                    SoundEngine::playJump();
                    // Normal jump logic...
                    m_InJump = true;
                    m_JumpClock.restart();
                    m_JumpsRemaining--;
                    m_JumpKeyReleased = false;
                }
            }
        }

        if (event.type == Event::KeyReleased) {
            if (event.key.code == Keyboard::D) {
                m_RightIsHeldDown = false;
            }
            if (event.key.code == Keyboard::A) {
                m_LeftIsHeldDown = false;
            }

            //. Allow the player to jump again once they let go of Space
            if (event.key.code == Keyboard::Space) {
                m_JumpKeyReleased = true;
                // Cut the jump short if key is released early
                // (Variable Jump Height)
                // m_InJump = false;
            }
        }
    }
    m_InputReceiver.clearEvents();
}

// Main Update Loop
void PlayerUpdate::update(float timeTakenThisFrame)
{
    if (m_IsPaused && *m_IsPaused)
        return;

    // Handle invincibility timer
    if (m_IsInvincible &&
        m_InvincibleClock.getElapsedTime().asSeconds() > m_InvincibleTime) {
        m_IsInvincible = false;
    }

    // Update flight timer
    if (m_FlightActive) {
        if (m_FlightClock.getElapsedTime().asSeconds() > m_FlightDuration) {
            m_FlightActive = false;
        }
    }

    // Instead of always applying gravity:
    if (!m_FlightActive && !m_IsClimbing) {
        m_Position.top += m_Gravity * timeTakenThisFrame;
    }

    // If grounded, reset double jump counter
    if (m_IsGrounded) {
        m_JumpsRemaining = MAX_JUMPS;
        m_InJump = false;
    }

    // Handle input before determine speed
    handleInput();

    // DYNAMIC HITBOX RESIZING
    if (m_IsDucking && !m_WasDucking) {
        // Shrink!
        m_Position.height = PLAYER_HEIGHT / 2.f;
        m_Position.top +=
            PLAYER_HEIGHT / 2.f; // Move top down so feet stay on ground
        m_WasDucking = true;
    } else if (!m_IsDucking && m_WasDucking) {
        // Grow back!
        m_Position.top -= PLAYER_HEIGHT / 2.f; // Move top up
        m_Position.height = PLAYER_HEIGHT;
        m_WasDucking = false;
    }

    // Determine Speed (Sprint vs Walk)
    m_CurrentSpeed = m_SprintIsHeldDown ? m_SprintSpeed : m_WalkSpeed;

    //. Horizontal movement (you can optionally lock X while climbing)
    if (m_IsClimbing) {
        // Lock to tree center
        m_Position.left = m_ClimbAnchorX;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            m_Position.top -= m_ClimbSpeed * timeTakenThisFrame;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            m_Position.top += m_ClimbSpeed * timeTakenThisFrame;
        }
    } else {
        // Non-climbing movement
        if (m_RightIsHeldDown) {
            m_Position.left += timeTakenThisFrame * m_CurrentSpeed;
        }
        if (m_LeftIsHeldDown) {
            m_Position.left -= timeTakenThisFrame * m_CurrentSpeed;
        }
    }

    //. If flight is active, allow vertical control with keys
    if (m_FlightActive) {
        // Hold Space to move up, S to move down (tweak keys if you want)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            m_Position.top -= m_FlightSpeed * timeTakenThisFrame;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            m_Position.top += m_FlightSpeed * timeTakenThisFrame;
        }
    }

    //. Handle Jumping Physics
    if (m_InJump) {
        // Simple Jump Arc
        if (m_JumpClock.getElapsedTime().asSeconds() < m_JumpDuration / 2) {
            // Going up
            m_Position.top -= m_JumpSpeed * timeTakenThisFrame;
        } else {
            // Going down
            m_Position.top += m_JumpSpeed * timeTakenThisFrame;
        }

        if (m_JumpClock.getElapsedTime().asSeconds() > m_JumpDuration) {
            m_InJump = false;
        }
    }
    m_IsGrounded = false; // Will be set to true by collision logic next frame
}
