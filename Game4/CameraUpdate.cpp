#include "CameraUpdate.hpp"
#include "PlayerUpdate.hpp"

using namespace sf;
using namespace std;

// Returns pointer to camera position
sf::FloatRect *CameraUpdate::getPositionPointer()
{
    return &m_Position;
}

// Assembles pointers to other updates
void CameraUpdate::assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                            std::shared_ptr<PlayerUpdate> playerUpdate)
{
    m_PlayerPosition = playerUpdate->getPositionPointer();
}

// Receives input from InputManager
InputReceiver *CameraUpdate::getInputReceiver()
{
    m_InputReceiver = new InputReceiver;
    m_ReceivesInput = true;
    return m_InputReceiver;
}

// Handles input events
void CameraUpdate::handleInput()
{
    m_Position.width = 1.0f;

    for (const sf::Event &event : m_InputReceiver->getEvents()) {
        // Handle mouse wheel event for zooming
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                m_Position.width *=
                    (event.mouseWheelScroll.delta > 0) ? 0.95f : 1.05f;
            }
        }
    }

    m_InputReceiver->clearEvents();
}

// Updates camera position based on player position
void CameraUpdate::update(float fps)
{
    if (m_ReceivesInput) {
        handleInput();
    }

    m_Position.left = m_PlayerPosition->left;
    m_Position.top = m_PlayerPosition->top;
}