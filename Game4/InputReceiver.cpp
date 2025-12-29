#include "InputReceiver.hpp"

using namespace sf;
using namespace std;

// Adds an event to the internal event list
void InputReceiver::addEvent(Event event)
{
    mEvents.push_back(event);
}

// Returns a reference to the vector of stored events
vector<Event> &InputReceiver::getEvents()
{
    return mEvents;
}

// Clears all stored events
void InputReceiver::clearEvents()
{
    mEvents.clear();
}
