#include "Ghost.hpp"
#include "GhostArena.hpp"

using namespace sf;

// Create an array of Ghosts for the haunting
Ghost *createHaunting(int numGhosts, IntRect arena)
{
    Ghost *ghosts = new Ghost[numGhosts];

    int maxY = arena.height - 20;
    int minY = arena.top + 20;
    int maxX = arena.width - 20;
    int minX = arena.left + 20;

    for (int i = 0; i < numGhosts; i++) {

        //. Which side should the Ghost spawn
        srand((int)time(0) * i);
        int side = (rand() % 4);
        float x, y;
        switch (side) {
            case 0:
                // left
                x = minX;
                y = (rand() % maxY) + minY;
                break;
            case 1:
                // right
                x = maxX;
                y = (rand() % maxY) + minY;
                break;
            case 2:
                // top
                x = (rand() % maxX) + minX;
                y = minY;
                break;
            case 3:
                // bottom
                x = (rand() % maxX) + minX;
                y = maxY;
                break;
        }

        //. Choose ghost type: white, green, blue, red
        // with different spawn probabilities
        srand((int)time(0) * i * 2);
        int roll = rand() % 10; // 0–9

        int type;
        if (roll < 5) {
            type = 0; // 50% White - common
        } else if (roll < 8) {
            type = 1; // 30% Green - slimer
        } else if (roll < 9) {
            type = 2; // 10% Blue - fast
        } else {
            type = 3; // 10% Red - evil jerk
        }

        //. Spawn the new Ghost into the array
        ghosts[i].spawn(x, y, type, i);
    }

    return ghosts;
}
