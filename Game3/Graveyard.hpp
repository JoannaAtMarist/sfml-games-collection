#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

// Spawns gravestone sprites and their collision boxes inside the arena.
//  - stones: sprites to draw
//  - stoneBounds: rectangles used for collision
//  - arena: same IntRect passed to createBackground / createHaunting
//  - tileSize: 64
void createGraveyard(std::vector<sf::Sprite> &stones,
                     std::vector<sf::FloatRect> &stoneBounds, sf::IntRect arena,
                     int tileSize);