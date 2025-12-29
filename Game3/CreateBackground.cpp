#include "GhostArena.hpp"

using namespace sf;

// Create the background as a vertex array of quads.
int createBackground(VertexArray &rVA, IntRect arena)
{
    // Size of each tile (both in world coordinates and texture coords)
    const int TILE_SIZE = 64;

    // Number of *floor* tile variations stacked vertically in the sheet.
    // (Rows 0,1,2 are floors, row 3 is wall.)
    const int TILE_TYPES = 3;

    // Each quad = 4 vertices
    const int VERTS_IN_QUAD = 4;

    // How many tiles wide and high is the arena, in tile units?
    int worldWidth = arena.width / TILE_SIZE;
    int worldHeight = arena.height / TILE_SIZE;

    // draw a big list of quads (4 verts per tile)
    rVA.setPrimitiveType(Quads);

    // Total vertices = number of tiles * 4
    rVA.resize(worldWidth * worldHeight * VERTS_IN_QUAD);

    // Index of the current vertex in the VertexArray
    int currentVertex = 0;

    // Loop over each tile position in the arena grid
    for (int w = 0; w < worldWidth; w++) {
        for (int h = 0; h < worldHeight; h++) {

            // 1) Set the *positions* of this tile's 4 vertices
            //
            // World space: each tile is TILE_SIZE wide/high,
            // and we place them in a grid starting at (0,0).

            rVA[currentVertex + 0].position =
                Vector2f(w * TILE_SIZE, h * TILE_SIZE); // top-left

            rVA[currentVertex + 1].position = Vector2f(
                (w * TILE_SIZE) + TILE_SIZE, h * TILE_SIZE); // top-right

            rVA[currentVertex + 2].position =
                Vector2f((w * TILE_SIZE) + TILE_SIZE,
                         (h * TILE_SIZE) + TILE_SIZE); // bottom-right

            rVA[currentVertex + 3].position =
                Vector2f((w * TILE_SIZE),
                         (h * TILE_SIZE) + TILE_SIZE); // bottom-left

            // 2) Set the *texture coordinates* for this quad
            //
            // This decides which 50x50 region of background_sheet.png
            // is drawn on this tile.

            // Edge tiles (top row, bottom row, left col, right col)
            // become walls.
            if (h == 0 || h == worldHeight - 1 || w == 0 ||
                w == worldWidth - 1) {

                // WALL TILE:
                // The wall is in the 4th row (index 3) of the sheet.
                // vertical offset in texture = 3 * TILE_SIZE.
                float wallYOffset = TILE_TYPES * TILE_SIZE; // 3 * 50 = 150

                rVA[currentVertex + 0].texCoords =
                    Vector2f(0, wallYOffset); // top-left of wall tile

                rVA[currentVertex + 1].texCoords =
                    Vector2f(TILE_SIZE, wallYOffset); // top-right

                rVA[currentVertex + 2].texCoords = Vector2f(
                    TILE_SIZE, wallYOffset + TILE_SIZE); // bottom-right

                rVA[currentVertex + 3].texCoords =
                    Vector2f(0, wallYOffset + TILE_SIZE); // bottom-left
            } else {
                // FLOOR TILE:
                // Each row is a different floor graphic.

                srand((int)time(0) + h * w - h);
                int mOrG = (rand() % TILE_TYPES); // 0, 1, or 2

                // verticalOffset = which row * tile height
                float verticalOffset = mOrG * TILE_SIZE; // 0, 50, or 100

                rVA[currentVertex + 0].texCoords =
                    Vector2f(0, verticalOffset); // top-left

                rVA[currentVertex + 1].texCoords =
                    Vector2f(TILE_SIZE, verticalOffset); // top-right

                rVA[currentVertex + 2].texCoords = Vector2f(
                    TILE_SIZE, verticalOffset + TILE_SIZE); // bottom-right

                rVA[currentVertex + 3].texCoords =
                    Vector2f(0, verticalOffset + TILE_SIZE); // bottom-left
            }

            // Move the "cursor" forward to the next group of 4 vertices
            currentVertex += VERTS_IN_QUAD;
        }
    }

    // Tell the caller how big a tile is (used for player spawn, etc.)
    return TILE_SIZE;
}
