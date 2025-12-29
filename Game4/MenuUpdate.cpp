#include "MenuUpdate.hpp"
#include "LevelUpdate.hpp"
#include "PlayerUpdate.hpp"
#include "SoundEngine.hpp"

using namespace sf;
using namespace std;

// Constructor
MenuUpdate::MenuUpdate(RenderWindow *window)
{
    m_Window = window;
}

// Getters
FloatRect *MenuUpdate::getPositionPointer()
{
    return &m_Position;
}
bool *MenuUpdate::getGameOverPointer()
{
    return &m_GameOver;
}
InputReceiver *MenuUpdate::getInputReceiver()
{
    return &m_InputReceiver;
}
bool *MenuUpdate::getIsPausedPointer()
{
    return m_IsPaused;
}
bool *MenuUpdate::getGameWonPointer()
{
    return m_GameWon;
}

// Assemble
void MenuUpdate::assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                          std::shared_ptr<PlayerUpdate> playerUpdate)
{
    // Save the full LevelUpdate pointer so we can call resetGame()
    m_LevelUpdate = levelUpdate.get();

    m_PlayerPosition = playerUpdate->getPositionPointer();
    m_IsPaused = levelUpdate->getIsPausedPointer();
    m_GameWon = levelUpdate->getGameWonPointer();

    m_Position.width = 75;
    m_Position.height = 75;

    // Initialize Position to 0 so it passes the (pos->left >= 0) check in
    // MenuGraphics immediately.
    m_Position.left = 0;
    m_Position.top = 0;

    SoundEngine::startMusic();
    // SoundEngine::pauseMusic(); // Keep this commented unless you want it
    //  silent by default

    m_IsVisible = true;
    *m_IsPaused = true;
    m_GameOver = true;
}

// Handle Input
void MenuUpdate::handleInput()
{
    for (const Event &event : m_InputReceiver.getEvents()) {
        if (event.type == Event::KeyPressed) {
            if (event.key.code == Keyboard::F1 && m_IsVisible) {
            }
        }

        if (event.type == Event::KeyReleased) {
            if (event.key.code == Keyboard::Enter) {

                // No longer the first load once we press Enter
                if (m_FirstLoad)
                    m_FirstLoad = false;

                // CASE 1: VICTORY - Full Reset
                if (m_GameWon && *m_GameWon) {
                    m_LevelUpdate->resetGame();
                    m_IsVisible = false;
                    *m_IsPaused = false;
                    m_GameOver = false; // Reset local flag
                }
                // CASE 2: NORMAL PAUSE / START SCREEN / GAME OVER
                else {
                    m_IsVisible = !m_IsVisible;
                    *m_IsPaused = !*m_IsPaused;

                    if (m_GameOver) {
                        m_GameOver = false;
                    }
                }
            }
        }
    }

    m_InputReceiver.clearEvents();
}

// Update
void MenuUpdate::update(float fps)
{
    handleInput();

    // Logic to detect if we died OR if we won
    if (*m_IsPaused && !m_IsVisible) {
        // If we paused but menu is hidden, show it
        m_IsVisible = true;

        // If it wasn't a victory, assume it's a Game Over (death)
        if (!(*m_GameWon)) {
            m_GameOver = true;
        }
    }

    if (m_IsVisible) {
        // Follow the player
        m_Position.left =
            m_PlayerPosition->getPosition().x - m_Position.width / 2;
        m_Position.top =
            m_PlayerPosition->getPosition().y - m_Position.height / 2;
    } else {
        m_Position.left = -999;
        m_Position.top = -999;
    }
}
