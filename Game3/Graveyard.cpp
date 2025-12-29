#include <cstdlib>
#include <ctime>

#include "Graveyard.hpp"
#include "TextureHolder.hpp"

using namespace sf;

// Function to create gravestones in the graveyard area
void createGraveyard(std::vector<Sprite> &stones,
                     std::vector<FloatRect> &stoneBounds, IntRect arena,
                     int tileSize)
{
    stones.clear();
    stoneBounds.clear();

    // Use the gravestones tilesheet (4 tiles high, 1 tile wide, 64x64 each)
    Texture &tex = TextureHolder::GetTexture("graphics/gravestones.png");

    const int NUM_ROWS = 4;
    const int STONE_SIZE = tileSize;

    // keep stones away from the walls
    // margin of 2 tiles on each side
    int margin = tileSize * 2;

    int minX = arena.left + margin;
    int maxX = arena.left + arena.width - margin;
    int minY = arena.top + margin;
    int maxY = arena.top + arena.height - margin;

    // Treat the center of the arena as "player start area"
    float playerX = arena.left + arena.width / 2.f;
    float playerY = arena.top + arena.height / 2.f;

    // Minimum distance from player start (in pixels)
    float minDistFromPlayer = tileSize * 3.f; // about 3 tiles away
    float minDistSq = minDistFromPlayer * minDistFromPlayer;

    // How many gravestones to spawn per wave
    int numStones = 6;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int i = 0; i < numStones; ++i) {
        Sprite s;
        s.setTexture(tex);

        // Pick one of the 4 gravestone tiles
        int row = std::rand() % NUM_ROWS;
        s.setTextureRect(IntRect(0, row * STONE_SIZE, STONE_SIZE, STONE_SIZE));

        // Random position inside the safe area
        int x = 0;
        int y = 0;
        int attempts = 0;

        do {
            x = minX + (std::rand() % std::max(1, maxX - minX));
            y = minY + (std::rand() % std::max(1, maxY - minY));

            float dx = static_cast<float>(x) - playerX;
            float dy = static_cast<float>(y) - playerY;
            float distSq = dx * dx + dy * dy;

            if (distSq >= minDistSq) {
                break; // far enough, use this position
            }

            attempts++;
        } while (attempts < 10);

        // Center-based, like player/ghosts
        s.setOrigin(STONE_SIZE / 2.f, STONE_SIZE / 2.f);
        s.setPosition(static_cast<float>(x), static_cast<float>(y));

        stones.push_back(s);

        // Start from the sprite bounds
        sf::FloatRect bounds = s.getGlobalBounds();

        // Shrink the box inward slightly so it hugs the visible stone
        const float inset = 8.f;
        bounds.left += inset;
        bounds.top += inset;
        bounds.width -= inset * 2.f;
        bounds.height -= inset * 2.f;

        stoneBounds.push_back(bounds);
    }
}