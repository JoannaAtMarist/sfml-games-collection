#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "InputReceiver.hpp"

// -------------------------------------------------------------
// InputDispatcher
//  - Collects SFML window events once per frame.
//  - Delivers each event to all registered InputReceivers.
// -------------------------------------------------------------
class InputDispatcher
{
  private:
    sf::RenderWindow *m_Window = nullptr;
    std::vector<InputReceiver *> m_InputReceivers;

  public:
    InputDispatcher(sf::RenderWindow *window);

    void dispatchInputEvents();
    void registerNewInputReceiver(InputReceiver *ir);
};