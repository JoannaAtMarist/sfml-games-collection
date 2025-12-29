#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "GameObject.hpp"
#include "Factory.hpp"
#include "InputDispatcher.hpp"
#include "SoundEngine.hpp"
#include "TextureHolder.hpp"

using namespace std;
using namespace sf;

int main()
{
    // Create a not-fullscreen window.
    RenderWindow window(VideoMode::getDesktopMode(), "Billina returns to Oz",
                        Style::Default);

    // Limit framerate for stability
    window.setFramerateLimit(65);

    // Create the SoundEngine (Loads buffers and prepares music)
    SoundEngine soundEngine;

    // This can dispatch events to any object.
    InputDispatcher inputDispatcher(&window);

    // Everything will be a game object.
    // This vector will hold them all.
    vector<GameObject *> gameObjects;

    // This class has all the knowledge
    // to construct game objects that do
    // many different things.
    Factory factory(&window);

    // This call will send the vector of game objects
    // the canvas to draw on and the input dispatcher
    // to the factory to set up the game.
    factory.loadLevel(gameObjects, inputDispatcher);

    // A clock for timing.
    Clock clock;

    // The color we use for the background
    const Color BACKGROUND_COLOR(100, 100, 100, 255);

    // The main game loop.
    while (window.isOpen()) {
        // Measure the time taken this frame.
        float timeTakenInSeconds = clock.restart().asSeconds();

        // Handle the player input.
        inputDispatcher.dispatchInputEvents();

        // Clear the previous frame.
        window.clear(BACKGROUND_COLOR);

        // Update all the game objects.
        for (auto *gameObject : gameObjects) {
            gameObject->update(timeTakenInSeconds);
        }

        // Draw all the game objects to the window.
        for (auto *gameObject : gameObjects) {
            gameObject->draw(window);
        }

        // Show the new frame.
        window.display();
    }

    return 0;
}