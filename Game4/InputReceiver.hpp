#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

// -------------------------------------------------------------
// InputReceiver
//  - Stores events given to it by InputDispatcher.
//  - Each Update component can read and process its own events.
// -------------------------------------------------------------
class InputReceiver
{
  private:
    std::vector<sf::Event> mEvents;

  public:
    void addEvent(sf::Event event);
    std::vector<sf::Event> &getEvents();
    void clearEvents();
};