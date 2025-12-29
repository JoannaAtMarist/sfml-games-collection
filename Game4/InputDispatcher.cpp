#include "InputDispatcher.hpp"

using namespace sf;
using namespace std;

// Constructor: initialize with the render window to poll events from
InputDispatcher::InputDispatcher(RenderWindow *window)
{
    m_Window = window;
}

// Dispatch all input events to all registered input receivers
void InputDispatcher::dispatchInputEvents()
{
    sf::Event event;
    while (m_Window->pollEvent(event))
    {

        // Basic global event: allow ESC to close the window
        if (event.type == Event::KeyPressed &&
            event.key.code == Keyboard::Escape)
        {
            m_Window->close();
        }

        // Send this event to EVERY registered receiver
        for (const auto &ir : m_InputReceivers)
        {
            ir->addEvent(event);
        }
    }
}

// Register a new input receiver to receive future input events
void InputDispatcher::registerNewInputReceiver(InputReceiver *ir)
{
    m_InputReceivers.push_back(ir);
}