#pragma once
#include <SFML/Graphics.hpp>

#include "Ghost.hpp"

// Create the background as a vertex array
int createBackground(sf::VertexArray &rVA, sf::IntRect arena);

// Create an array of Ghosts
Ghost *createHaunting(int numGhosts, sf::IntRect arena);
