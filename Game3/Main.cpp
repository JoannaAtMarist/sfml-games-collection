#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Bullet.hpp"
#include "Pickup.hpp"
#include "Player.hpp"

#include "GhostArena.hpp"
#include "Graveyard.hpp"

#include "TextureHolder.hpp"

using namespace sf;

//. Game state enum
// The game can only ever be in one of these four modes.
enum class State
{
    PAUSED,
    LEVELING_UP,
    GAME_OVER,
    PLAYING
};

//. Default haunted room size (world coordinates)
const int ROOM_WIDTH = 1800;
const int ROOM_HEIGHT = 1400; // roughly 16:9

//. Enemy contact damage per ghost type
const int WHITE_GHOST_DAMAGE = 10;
const int GREEN_GHOST_DAMAGE = 8; // a bit gentler on contact
const int BLUE_GHOST_DAMAGE = 10; // normal
const int RED_GHOST_DAMAGE = 18;  // hits harder

//. Green "Slimer" explosion on death
const int GREEN_EXPLOSION_DAMAGE = 20;
const float GREEN_EXPLOSION_RADIUS = 120.f; // adjustable

//. struct Game3State
// Central "world state" for the game.
// Instead of having a pile of local variables in main(), everything that
// describes the current game lives here: window, views, player, ghosts,
// bullets, HUD, audio, and a few debug options.
struct Game3State
{
    // Core
    State state = State::GAME_OVER;
    Vector2f resolution;
    RenderWindow window;
    View mainView; // world-space view (player, ghosts, background)
    View hudView;  // screen-space view (HUD text, bars)
    Clock clock;   // per-frame timing
    Time gameTimeTotal;

    // Mouse + player
    Vector2f mouseWorldPosition;  // mouse in world coordinates (for aiming)
    Vector2i mouseScreenPosition; // mouse in window coordinates
    Player player;
    IntRect arena; // current arena rectangle

    // Background
    VertexArray background;
    Texture textureBackground;

    // Obstacles: gravestones that block movement
    std::vector<sf::Sprite> gravestoneSprites;
    std::vector<sf::FloatRect> gravestoneBounds;

    // Ghosts
    int numGhosts = 0;
    int numGhostsActive = 0;
    Ghost *ghosts = nullptr; // dynamically allocated haunting

    // Bullets
    Bullet bullets[100];
    int currentBullet = 0;
    int bulletsSpare = 24;
    int bulletsInClip = 6;
    int clipSize = 6;
    float fireRate = 1.f; // bullets per second
    Time lastPressed;     // last time the mouse fired

    // Crosshair
    Sprite spriteCrosshair;
    Texture textureCrosshair;

    // Pickups
    Pickup healthPickup{1};
    Pickup ammoPickup{2};

    // Game stats
    int score = 0;
    int hiScore = 0;
    int wave = 0;

    // Special actions
    int nukesRemaining = 1;  // quantity-limited
    int maxNukes = 3;        // cap for Ghost Grenades
    float damageScale = 1.f; // 1.0 = normal damage, <1 = resistant

    // Special pickups
    Sprite shieldPickupSprite;
    Texture shieldPickupTexture;
    bool shieldPickupSpawned = false;

    Sprite freezePickupSprite;
    Texture freezePickupTexture;
    bool freezePickupSpawned = false;

    // Active temporary effects
    bool shieldActive = false;
    float shieldTimer = 0.f; // seconds left of invincibility

    bool freezeActive = false;
    float freezeTimer = 0.f; // seconds left of freeze

    // HUD sprites/text
    Sprite spriteGameOver;
    Texture textureGameOver;

    Sprite spriteAmmoIcon;
    Texture textureAmmoIcon;

    Font font;
    Text pausedText;
    Text gameOverText;
    Text levelUpText;
    Text ammoText;
    Text scoreText;
    Text hiScoreText;
    Text ghostsRemainingText;
    Text waveNumberText;
    RectangleShape healthBar;
    Text nukesText;

    int framesSinceLastHUDUpdate = 0;
    int fpsMeasurementFrameInterval = 1000; // how often HUD text is refreshed

    // Sounds
    SoundBuffer hitBuffer, splatBuffer, shootBuffer, reloadBuffer,
        reloadFailedBuffer, powerupBuffer, pickupBuffer;
    Sound hit, splat, shoot, reload, reloadFailed, powerup, pickup;
    Music backgroundMusic;

    // DEBUG: hitbox settings
    // (for circle-based collision + on-screen outlines)
    bool debugHitboxes = false;
    float playerHitRadius = 20.f;
    float ghostHitRadius = 20.f;

}; // End Game3State struct

//. Game Loop function declarations
/**
 * Initialize the Game3State:
 *  - Creates the window and views
 *  - Loads textures, fonts, and sounds
 *  - Sets up HUD text objects and base stats
 */
void initializeGame(Game3State &g);
/**
 * Main game loop:
 *  - While the window is open, process input, update world state,
 *    and render the scene.
 */
void gameLoop(Game3State &g);
/**
 * Handle all player input for a single frame:
 *  - Window events (close, Enter, etc.)
 *  - State transitions (PAUSED, LEVELING_UP, GAME_OVER -> PLAYING)
 *  - Movement, firing, reloading, dash
 *  - Debug hotkeys (hitbox toggle)
 */
void handleInput(Game3State &g);
/**
 * Advance the simulation for one frame:
 *  - Compute delta time
 *  - Update mouse, player, ghosts, bullets, and pickups
 *  - Run collision detection
 *  - Refresh HUD text and health bar
 */
void updateGame(Game3State &g);
/**
 * Draw the current frame:
 *  - World layer (background, ghosts, bullets, pickups, player)
 *  - Optional debug overlays (circle hitboxes)
 *  - HUD layer (ammo, score, wave, health, etc.)
 */
void renderScene(Game3State &g);
/**
 * Clean up any dynamic resources and write persistent data:
 *  - Save hi-score to disk
 *  - Delete ghost array
 */
void finalizeGame(Game3State &g);
/**
 * Collision system for a single frame:
 *  - Bullet vs. ghost (axis-aligned rectangles)
 *  - Player vs. ghost (circle hitboxes)
 *  - Player vs. pickups (rectangles)
 */
void checkCollisions(Game3State &g);

// Main function
int main()
{
    // The instance of TextureHolder
    TextureHolder holder;

    Game3State game;
    initializeGame(game);
    gameLoop(game);
    finalizeGame(game);

    return 0;
}
// End of main

// Game Loop function definitions
void gameLoop(Game3State &g)
{
    while (g.window.isOpen()) {
        handleInput(g);

        if (!g.window.isOpen()) {
            // Window was closed during input handling
            break;
        }

        updateGame(g);
        renderScene(g);
    }
}
// End of gameLoop

// Handle all player input for a single frame
void handleInput(Game3State &g)
{
    // Events (Enter, R, git pull, leveling-up choices)
    Event event;

    //. Start of pollEvent
    while (g.window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            g.window.close();
            return;
        }

        //. Start of Event::KeyPressed
        if (event.type == Event::KeyPressed) {

            // DEBUG: Toggle hitbox debug drawing with H
            if (event.key.code == Keyboard::H) {
                g.debugHitboxes = !g.debugHitboxes;
            }

            // Pause while playing
            if (event.key.code == Keyboard::Return &&
                g.state == State::PLAYING) {
                g.state = State::PAUSED;
            }
            // Restart while paused
            else if (event.key.code == Keyboard::Return &&
                     g.state == State::PAUSED) {
                g.state = State::PLAYING;
                g.clock.restart();
            }
            // Start new game from GAME_OVER
            else if (event.key.code == Keyboard::Return &&
                     g.state == State::GAME_OVER) {
                g.state = State::LEVELING_UP;
                g.wave = 0;
                g.score = 0;

                g.currentBullet = 0;
                g.bulletsSpare = 24;
                g.bulletsInClip = 6;
                g.clipSize = 6;
                g.fireRate = 1.f;
                g.player.resetPlayerStats();

                // Special action resets
                g.nukesRemaining = 1;
                g.damageScale = 1.f; // back to normal damage
            }

            // Reloading while playing
            if (g.state == State::PLAYING && event.key.code == Keyboard::R) {

                if (g.bulletsSpare >= g.clipSize) {
                    g.bulletsInClip = g.clipSize;
                    g.bulletsSpare -= g.clipSize;
                } else if (g.bulletsSpare > 0) {
                    g.bulletsInClip = g.bulletsSpare;
                    g.bulletsSpare = 0;
                } else {
                    g.reloadFailed.play();
                }
            }

            // Dash (temporary speed boost)
            if (g.state == State::PLAYING &&
                event.key.code == Keyboard::LShift) {
                g.player.triggerDash();
            }

            // Nuke: quantity-limited action
            // with DEBUG output
            if (g.state == State::PLAYING && event.key.code == Keyboard::E) {

                if (g.nukesRemaining > 0) {

                    // Count how many ghosts are currently active
                    int activeCount = 0;
                    for (int i = 0; i < g.numGhosts; ++i) {
                        if (g.ghosts[i].isActive()) {
                            activeCount++;
                        }
                    }

                    int ghostsToKill = activeCount / 2;
                    int ghostsKilledThisNuke = 0;

                    for (int i = 0; i < g.numGhosts && ghostsToKill > 0; ++i) {
                        if (!g.ghosts[i].isActive())
                            continue;

                        // Repeatedly hit this ghost until it dies
                        // Ghost::hit() returns true when health drops
                        // below 0
                        while (g.ghosts[i].isActive()) {
                            if (g.ghosts[i].hit()) {
                                // We just killed it
                                g.score += 10;
                                if (g.score >= g.hiScore) {
                                    g.hiScore = g.score;
                                }
                                g.numGhostsActive--;
                                ghostsKilledThisNuke++;
                                break; // done with this ghost
                            }
                        }

                        ghostsToKill--;
                    }

                    // If that killed the last ghost, go to LEVELING_UP
                    if (g.numGhostsActive == 0) {
                        g.state = State::LEVELING_UP;
                    }

                    // Feedback
                    g.powerup.play();
                    g.nukesRemaining--;

                    // Immediately refresh nukes HUD text
                    std::stringstream nukesImmediate;
                    nukesImmediate << "Nukes:" << g.nukesRemaining;
                    g.nukesText.setString(nukesImmediate.str());

                    // Console debug
                    std::cout
                        << "[DEBUG] Nuke used: killed " << ghostsKilledThisNuke
                        << " ghosts. Ghosts remaining: " << g.numGhostsActive
                        << std::endl;
                }
            } // End Nuke with debug

            //. UPGRADE KEYS
            if (g.state == State::LEVELING_UP) {

                // 1 - Increased rate of fire
                if (event.key.code == Keyboard::Num1) {
                    g.fireRate += 1.f;
                    std::cout << "[DEBUG] Upgrade: fire rate now " << g.fireRate
                              << "\n";
                    g.state = State::PLAYING;
                }
                // 2 - Increased run speed
                else if (event.key.code == Keyboard::Num2) {
                    g.player.upgradeSpeed();
                    std::cout << "[DEBUG] Upgrade: run speed increased\n";
                    g.state = State::PLAYING;
                }
                // 3 - Resistance (take reduced damage)
                else if (event.key.code == Keyboard::Num3) {
                    g.damageScale *= 0.75f; // 25% less damage each time
                    if (g.damageScale < 0.25f) {
                        g.damageScale = 0.25f; // cap at 75% reduction
                    }
                    std::cout << "[DEBUG] Upgrade: damageScale now "
                              << g.damageScale << "\n";
                    g.state = State::PLAYING;
                }
                // 4 - Ghost Grenade refill (+1 up to cap)
                else if (event.key.code == Keyboard::Num4) {
                    if (g.nukesRemaining < g.maxNukes) {
                        g.nukesRemaining++;
                        std::cout << "[DEBUG] Upgrade: nukesRemaining = "
                                  << g.nukesRemaining << " (max " << g.maxNukes
                                  << ")\n";
                    } else {
                        std::cout << "[DEBUG] Upgrade: nukes already at cap ("
                                  << g.maxNukes << ")\n";
                    }
                    g.state = State::PLAYING;
                }

                //. WAVE SETUP
                if (g.state == State::PLAYING) {
                    // Wave setup:
                    //  - Expand arena
                    //  - Rebuild background
                    //  - Respawn player and pickups
                    //  - Create a fresh haunting
                    g.wave++;

                    // Fixed-size haunted room for all waves (rectangular)
                    g.arena.width = ROOM_WIDTH;
                    g.arena.height = ROOM_HEIGHT;
                    g.arena.left = 0;
                    g.arena.top = 0;

                    int tileSize = createBackground(g.background, g.arena);
                    g.player.spawn(g.arena, g.resolution, tileSize);

                    // Make an inner rectangle so pickups
                    // never spawn on the wall tiles
                    sf::IntRect pickupArea = g.arena;
                    pickupArea.left += tileSize;
                    pickupArea.top += tileSize;
                    pickupArea.width -= tileSize * 2;
                    pickupArea.height -= tileSize * 2;

                    g.healthPickup.setArena(pickupArea);
                    g.ammoPickup.setArena(pickupArea);

                    // Build gravestone obstacles for this arena
                    createGraveyard(g.gravestoneSprites, g.gravestoneBounds,
                                    g.arena, tileSize);

                    g.numGhosts = 5 * g.wave;
                    delete[] g.ghosts;
                    g.ghosts = createHaunting(g.numGhosts, g.arena);
                    g.numGhostsActive = g.numGhosts;

                    // Spawn shield & freeze pickups at random spots
                    int minX = g.arena.left + 50;
                    int maxX = g.arena.left + g.arena.width - 50;
                    int minY = g.arena.top + 50;
                    int maxY = g.arena.top + g.arena.height - 50;

                    // Shield
                    {
                        int x = minX + (rand() % std::max(1, maxX - minX));
                        int y = minY + (rand() % std::max(1, maxY - minY));
                        g.shieldPickupSprite.setPosition(static_cast<float>(x),
                                                         static_cast<float>(y));
                        g.shieldPickupSpawned = true;
                    }

                    // Freeze
                    {
                        int x = minX + (rand() % std::max(1, maxX - minX));
                        int y = minY + (rand() % std::max(1, maxY - minY));
                        g.freezePickupSprite.setPosition(static_cast<float>(x),
                                                         static_cast<float>(y));
                        g.freezePickupSpawned = true;
                    }

                    g.powerup.play();
                    g.clock.restart();

                } // End State::PLAYING
            } // End State::LEVELING_UP
        } // End Event::KeyPressed
    } // End pollEvent

    //. Real-time input (WASD, Escape, firing)
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        g.window.close();
        return;
    }

    if (g.state == State::PLAYING) {
        // Movement (WASD)
        if (Keyboard::isKeyPressed(Keyboard::W)) {
            g.player.moveUp();
        } else {
            g.player.stopUp();
        }
        if (Keyboard::isKeyPressed(Keyboard::S)) {
            g.player.moveDown();
        } else {
            g.player.stopDown();
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            g.player.moveLeft();
        } else {
            g.player.stopLeft();
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            g.player.moveRight();
        } else {
            g.player.stopRight();
        }

        //. Fire a bullet
        if (Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (g.gameTimeTotal.asMilliseconds() -
                        g.lastPressed.asMilliseconds() >
                    1000 / g.fireRate &&
                g.bulletsInClip > 0) {
                // Pass the center of the player and
                // the center of the cross-hair to the shoot function
                g.bullets[g.currentBullet].shoot(
                    g.player.getCenter().x, g.player.getCenter().y,
                    g.mouseWorldPosition.x, g.mouseWorldPosition.y);
                g.currentBullet++;
                if (g.currentBullet > 99) {
                    g.currentBullet = 0;
                }
                g.lastPressed = g.gameTimeTotal;
                g.shoot.play();
                g.bulletsInClip--;
            }
        } // End fire a bullet
    } // End State::PLAYING
}
// End of handleInput

// Advance the simulation for one frame
void updateGame(Game3State &g)
{
    if (g.state != State::PLAYING) {
        return;
    }

    // Delta time since last frame
    Time dt = g.clock.restart();
    g.gameTimeTotal += dt;
    float dtAsSeconds = dt.asSeconds();

    // Mouse position (screen -> world)
    g.mouseScreenPosition = Mouse::getPosition(g.window);
    g.mouseWorldPosition =
        g.window.mapPixelToCoords(g.mouseScreenPosition, g.mainView);
    g.spriteCrosshair.setPosition(g.mouseWorldPosition);

    // Player movement & rotation with gravestone collision
    // Remember where the player was before moving
    sf::Vector2f oldPlayerCenter = g.player.getCenter();
    // Let the player move (uses WASD + dash)
    g.player.update(dtAsSeconds, g.mouseScreenPosition);
    // Check collision with gravestones
    sf::FloatRect playerBounds = g.player.getPosition();
    bool hitGravestone = false;

    for (const auto &stoneRect : g.gravestoneBounds) {
        if (playerBounds.intersects(stoneRect)) {
            hitGravestone = true;
            break;
        }
    }
    // If we collided, snap back to the previous safe position
    if (hitGravestone) {
        g.player.setCenter(oldPlayerCenter);
    }
    // Whatever the final center is, follow it with the camera
    sf::Vector2f playerPosition = g.player.getCenter();
    g.mainView.setCenter(playerPosition);

    // Ghosts chase the player (unless frozen)
    for (int i = 0; i < g.numGhosts; ++i) {
        if (!g.ghosts[i].isActive())
            continue;

        if (!g.freezeActive) {
            g.ghosts[i].update(dtAsSeconds, playerPosition);
        }
        // else: frozen in place; we skip movement update
    }

    // Bullets travel forward (movement + animation only)
    for (int i = 0; i < 100; ++i) {
        if (g.bullets[i].isInFlight()) {
            g.bullets[i].update(dtAsSeconds);
        }
    }

    // Pickups animate / respawn timers
    g.healthPickup.update(dtAsSeconds);
    g.ammoPickup.update(dtAsSeconds);

    // Shield & freeze timers
    if (g.shieldActive) {
        g.shieldTimer -= dtAsSeconds;
        if (g.shieldTimer <= 0.f) {
            g.shieldActive = false;
            g.shieldTimer = 0.f;
            std::cout << "[DEBUG] Shield expired\n";
        }
    }

    if (g.freezeActive) {
        g.freezeTimer -= dtAsSeconds;
        if (g.freezeTimer <= 0.f) {
            g.freezeActive = false;
            g.freezeTimer = 0.f;
            std::cout << "[DEBUG] Freeze expired\n";
        }
    }

    //. Collisions in separate function
    checkCollisions(g);

    // HUD update (runs every fpsMeasurementFrameInterval frames)
    g.framesSinceLastHUDUpdate++;
    if (g.framesSinceLastHUDUpdate > g.fpsMeasurementFrameInterval) {
        // Ammo
        std::stringstream ammoSs;
        ammoSs << g.bulletsInClip << "/" << g.bulletsSpare;
        g.ammoText.setString(ammoSs.str());

        // Score
        std::stringstream scoreSs;
        scoreSs << "Score:" << g.score;
        g.scoreText.setString(scoreSs.str());

        // Hi-score
        std::stringstream hiSs;
        hiSs << "Hi Score:" << g.hiScore;
        g.hiScoreText.setString(hiSs.str());

        // Wave
        std::stringstream waveSs;
        waveSs << "Wave:" << g.wave;
        g.waveNumberText.setString(waveSs.str());

        // Ghosts remaining
        std::stringstream zSs;
        zSs << "Ghosts:" << g.numGhostsActive;
        g.ghostsRemainingText.setString(zSs.str());

        // Nukes remaining
        std::stringstream nukesSs;
        nukesSs << "Nukes:" << g.nukesRemaining;
        g.nukesText.setString(nukesSs.str());

        g.framesSinceLastHUDUpdate = 0;
    }

    // Health bar size (3 pixels per HP)
    g.healthBar.setSize(Vector2f(g.player.getHealth() * 3, 50));
}
// End of updateGame

// Draw the current frame
void renderScene(Game3State &g)
{
    g.window.clear();

    if (g.state == State::PLAYING) {
        g.window.setView(g.mainView);

        // Background
        g.window.draw(g.background, &g.textureBackground);

        // Gravestone obstacles
        for (const auto &stone : g.gravestoneSprites) {
            g.window.draw(stone);
        }

        // Ghosts
        for (int i = 0; i < g.numGhosts; ++i) {
            g.window.draw(g.ghosts[i].getSprite());
        }

        // Bullets
        for (int i = 0; i < 100; ++i) {
            if (g.bullets[i].isInFlight()) {
                g.window.draw(g.bullets[i].getSprite());
            }
        }

        // Pick-ups
        if (g.ammoPickup.isSpawned()) {
            g.window.draw(g.ammoPickup.getSprite());
        }
        if (g.healthPickup.isSpawned()) {
            g.window.draw(g.healthPickup.getSprite());
        }

        // Special pickups
        if (g.shieldPickupSpawned) {
            g.window.draw(g.shieldPickupSprite);
        }
        if (g.freezePickupSpawned) {
            g.window.draw(g.freezePickupSprite);
        }

        // Crosshair & player
        g.window.draw(g.spriteCrosshair);
        g.window.draw(g.player.getSprite());

        // DEBUG: draw circle hitboxes
        if (g.debugHitboxes) {
            // Player circle
            CircleShape playerCircle;
            playerCircle.setRadius(g.playerHitRadius);
            playerCircle.setOrigin(g.playerHitRadius, g.playerHitRadius);
            playerCircle.setPosition(g.player.getCenter());
            playerCircle.setFillColor(Color::Transparent);
            playerCircle.setOutlineColor(Color::Magenta);
            playerCircle.setOutlineThickness(2.f);
            g.window.draw(playerCircle);

            // ghost circles
            for (int i = 0; i < g.numGhosts; ++i) {
                if (!g.ghosts[i].isActive())
                    continue;

                FloatRect zr = g.ghosts[i].getPosition();
                Vector2f zCenter(zr.left + zr.width / 2.f,
                                 zr.top + zr.height / 2.f);

                CircleShape zCircle;
                zCircle.setRadius(g.ghostHitRadius);
                zCircle.setOrigin(g.ghostHitRadius, g.ghostHitRadius);
                zCircle.setPosition(zCenter);
                zCircle.setFillColor(Color::Transparent);
                zCircle.setOutlineColor(Color::Cyan);
                zCircle.setOutlineThickness(2.f);
                g.window.draw(zCircle);
            }
        } // End DEBUG: draw circle hitboxes

        // HUD layer
        g.window.setView(g.hudView);
        g.window.draw(g.spriteAmmoIcon);
        g.window.draw(g.ammoText);
        g.window.draw(g.scoreText);
        g.window.draw(g.hiScoreText);
        g.window.draw(g.healthBar);
        g.window.draw(g.waveNumberText);
        g.window.draw(g.ghostsRemainingText);
        g.window.draw(g.nukesText); // Nukes

    } // End g.state == State::PLAYING

    // Overlay states (no world view needed)
    if (g.state == State::LEVELING_UP) {
        g.window.setView(g.hudView);
        g.window.draw(g.spriteGameOver);
        g.window.draw(g.levelUpText);
    } else if (g.state == State::PAUSED) {
        g.window.setView(g.hudView);
        g.window.draw(g.pausedText);
    } else if (g.state == State::GAME_OVER) {
        g.window.setView(g.hudView);
        g.window.draw(g.spriteGameOver);
        g.window.draw(g.gameOverText);
        g.window.draw(g.scoreText);
        g.window.draw(g.hiScoreText);
    }

    g.window.display();
}
// End of renderScene

// Initialize the Game3State
void initializeGame(Game3State &g)
{
    // Screen + window
    g.resolution.x = VideoMode::getDesktopMode().width;
    g.resolution.y = VideoMode::getDesktopMode().height;

    g.window.create(VideoMode(g.resolution.x, g.resolution.y), "Bhost Guster",
                    Style::Default);

    g.window.setMouseCursorVisible(true);

    // Limit framerate for stability
    g.window.setFramerateLimit(65);

    // Views
    g.mainView = View(FloatRect(0, 0, g.resolution.x, g.resolution.y));
    g.hudView = View(FloatRect(0, 0, g.resolution.x, g.resolution.y));

    // Background
    g.textureBackground =
        TextureHolder::GetTexture("graphics/background_sheet2.png");
    // arena is set later when LEVELING_UP happens
    // g.background is filled in createBackground when wave starts

    // Crosshair
    g.textureCrosshair = TextureHolder::GetTexture("graphics/crosshair068.png");
    g.spriteCrosshair.setTexture(g.textureCrosshair);
    // New texture is 128x128, so origin should be center (64,64)
    g.spriteCrosshair.setOrigin(64.f, 64.f);
    // Scale 128x128 down to roughly 55x55
    g.spriteCrosshair.setScale(55.f / 128.f, 55.f / 128.f);

    // Pickups
    // (arena also set later when wave starts)
    // g.healthPickup and g.ammoPickup already constructed with types 1 and 2

    // Game stats
    g.score = 0;
    g.hiScore = 0;

    // Gameover background
    g.textureGameOver = TextureHolder::GetTexture("graphics/background.png");
    g.spriteGameOver.setTexture(g.textureGameOver);
    g.spriteGameOver.setPosition(0, 0);

    // Font + texts
    g.font.loadFromFile("fonts/GHOSTBUS.TTF");

    // Paused text
    g.pausedText.setFont(g.font);
    g.pausedText.setCharacterSize(155);
    g.pausedText.setFillColor(Color::White);
    g.pausedText.setPosition(400, 400);
    g.pausedText.setString("Press Enter \nto continue");

    // Game over text
    g.gameOverText.setFont(g.font);
    g.gameOverText.setCharacterSize(125);
    g.gameOverText.setFillColor(Color::White);
    g.gameOverText.setPosition(250, 400);
    g.gameOverText.setString("Press Enter to play");

    // Level up text
    g.levelUpText.setFont(g.font);
    g.levelUpText.setCharacterSize(60);
    g.levelUpText.setFillColor(Color::White);
    g.levelUpText.setPosition(70, 250);
    std::stringstream levelUpStream;
    levelUpStream << "1 - Increased rate of fire"
                  << "\n2 - Increased run speed"
                  << "\n3 - Damage resistance (take less ghost damage)"
                  << "\n4 - Ghost Grenade refill (+1 up to cap)";
    g.levelUpText.setString(levelUpStream.str());

    // Ammo HUD
    g.ammoText.setFont(g.font);
    g.ammoText.setCharacterSize(50);
    g.ammoText.setFillColor(Color::White);
    g.ammoText.setPosition(70, 910);

    // Nukes HUD
    g.nukesText.setFont(g.font);
    g.nukesText.setCharacterSize(50);
    g.nukesText.setFillColor(Color::White);
    g.nukesText.setPosition(70, 1000);
    g.nukesText.setString("Nukes: 1");
    g.scoreText.setFont(g.font);
    g.scoreText.setCharacterSize(50);
    g.scoreText.setFillColor(Color::White);
    g.scoreText.setPosition(20, 0);

    // Load hi-score file
    std::ifstream inputFile("gamedata/scores.txt");
    if (inputFile.is_open()) {
        inputFile >> g.hiScore;
        inputFile.close();
    }

    // Hi-score HUD
    g.hiScoreText.setFont(g.font);
    g.hiScoreText.setCharacterSize(50);
    g.hiScoreText.setFillColor(Color::White);
    g.hiScoreText.setPosition(1400, 0);
    std::stringstream s;
    s << "High Score:" << g.hiScore;
    g.hiScoreText.setString(s.str());

    // Ghosts remaining HUD
    g.ghostsRemainingText.setFont(g.font);
    g.ghostsRemainingText.setCharacterSize(50);
    g.ghostsRemainingText.setFillColor(Color::White);
    g.ghostsRemainingText.setPosition(1500, 980);
    g.ghostsRemainingText.setString("Ghosts: 100");

    // Wave number HUD
    g.wave = 0;
    g.waveNumberText.setFont(g.font);
    g.waveNumberText.setCharacterSize(50);
    g.waveNumberText.setFillColor(Color::White);
    g.waveNumberText.setPosition(1250, 980);
    g.waveNumberText.setString("Wave: 0");

    // Shield pickup
    g.shieldPickupTexture =
        TextureHolder::GetTexture("graphics/pickups/shield_pickup.png");
    g.shieldPickupSprite.setTexture(g.shieldPickupTexture);
    {
        // Center origin based on real texture size
        sf::FloatRect bounds = g.shieldPickupSprite.getLocalBounds();
        g.shieldPickupSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    }

    // Freeze pickup
    g.freezePickupTexture =
        TextureHolder::GetTexture("graphics/pickups/freeze_pickup.png");
    g.freezePickupSprite.setTexture(g.freezePickupTexture);
    {
        sf::FloatRect bounds = g.freezePickupSprite.getLocalBounds();
        g.freezePickupSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    }

    g.shieldPickupSpawned = false;
    g.freezePickupSpawned = false;
    g.shieldActive = false;
    g.freezeActive = false;
    g.shieldTimer = 0.f;
    g.freezeTimer = 0.f;

    // Health HUD
    g.healthBar.setFillColor(Color::Red);
    g.healthBar.setPosition(400, 980);

    g.framesSinceLastHUDUpdate = 0;
    g.fpsMeasurementFrameInterval = 65;

    // Background music (looping)
    if (!g.backgroundMusic.openFromFile(
            "sound/mjpicciano-bhost-guster-jam.ogg")) {
        std::cout << "[WARN] Could not load background music.\n";
    } else {
        g.backgroundMusic.setLoop(true);
        g.backgroundMusic.setVolume(50.f); // adjustable
        g.backgroundMusic.play();
    }

    // Sound buffers & sounds
    g.hitBuffer.loadFromFile("sound/hit.wav");
    g.hit.setBuffer(g.hitBuffer);

    g.splatBuffer.loadFromFile("sound/classic-ghost-sound-95773.ogg");
    g.splat.setBuffer(g.splatBuffer);

    g.shootBuffer.loadFromFile("sound/KJH_NutronaCombo.ogg");
    g.shoot.setBuffer(g.shootBuffer);

    g.reloadBuffer.loadFromFile("sound/KJH_PackstartCombo.ogg");
    g.reload.setBuffer(g.reloadBuffer);

    g.reloadFailedBuffer.loadFromFile("sound/KJH_PackstopDigital.ogg");
    g.reloadFailed.setBuffer(g.reloadFailedBuffer);

    g.powerupBuffer.loadFromFile("sound/powerup.wav");
    g.powerup.setBuffer(g.powerupBuffer);

    g.pickupBuffer.loadFromFile("sound/pickup.wav");
    g.pickup.setBuffer(g.pickupBuffer);

    // Initial gun stats (same as GAME_OVER reset)
    g.currentBullet = 0;
    g.bulletsSpare = 24;
    g.bulletsInClip = 6;
    g.clipSize = 6;
    g.fireRate = 1.f;

    // Start on the GAME_OVER screen until the player presses Enter
    g.state = State::GAME_OVER;
}
// End of initializeGame

// Clean up any dynamic resources and write persistent data
void finalizeGame(Game3State &g)
{
    // Save hi score
    std::ofstream outputFile("gamedata/scores.txt");
    if (outputFile.is_open()) {
        outputFile << g.hiScore;
        outputFile.close();
    }

    delete[] g.ghosts;
    g.ghosts = nullptr;
}
// End of finalizeGame

// Collision system for a single frame
void checkCollisions(Game3State &g)
{
    //. Have any ghosts been shot? (bullet vs ghost rectangles)
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < g.numGhosts; j++) {
            if (g.bullets[i].isInFlight() && g.ghosts[j].isActive()) {
                if (g.bullets[i].getPosition().intersects(
                        g.ghosts[j].getPosition())) {
                    // Stop the bullet
                    g.bullets[i].stop();

                    // Remember which type we hit before we potentially kill it
                    int ghostType = g.ghosts[j].getType();

                    // Register the hit and see if it was a kill
                    if (g.ghosts[j].hit()) {

                        // Green "Slimer" explosion on death
                        if (ghostType == GHOST_GREEN) {
                            // Explosion center = ghost center
                            sf::FloatRect gr = g.ghosts[j].getPosition();
                            sf::Vector2f gCenter(gr.left + gr.width / 2.f,
                                                 gr.top + gr.height / 2.f);

                            sf::Vector2f playerCenter = g.player.getCenter();
                            float dx = playerCenter.x - gCenter.x;
                            float dy = playerCenter.y - gCenter.y;
                            float dist2 = dx * dx + dy * dy;
                            float radius2 =
                                GREEN_EXPLOSION_RADIUS * GREEN_EXPLOSION_RADIUS;

                            if (dist2 <= radius2 && !g.shieldActive) {
                                int adjustedDamage = static_cast<int>(
                                    std::round(GREEN_EXPLOSION_DAMAGE *
                                               g.damageScale));
                                if (adjustedDamage < 1)
                                    adjustedDamage = 1;

                                if (g.player.hit(g.gameTimeTotal,
                                                 adjustedDamage)) {
                                    g.hit.play();
                                }
                                std::cout << "[DEBUG] Slimer explosion hit "
                                             "player for "
                                          << adjustedDamage << " damage\n";

                            } else {
                                std::cout << "[DEBUG] Slimer exploded; player "
                                             "out of range or shielded\n";
                            }
                        }

                        // Not just a hit but a kill too
                        g.score += 10;
                        if (g.score >= g.hiScore) {
                            g.hiScore = g.score;
                        }
                        g.numGhostsActive--;
                        if (g.numGhostsActive == 0) {
                            g.state = State::LEVELING_UP;
                        }

                        g.splat.play();
                    }
                }
            }
        }
    } // End ghost being shot

    //. Have any ghosts touched the player (circle hitboxes)
    Vector2f playerCenter = g.player.getCenter();
    float playerR = g.playerHitRadius;
    float ghostR = g.ghostHitRadius;
    float sumR = playerR + ghostR;
    float sumR2 = sumR * sumR; // compare squared distances

    for (int i = 0; i < g.numGhosts; i++) {
        if (!g.ghosts[i].isActive())
            continue;

        // Approximate ghost center from global bounds
        FloatRect zr = g.ghosts[i].getPosition();
        Vector2f ghostCenter(zr.left + zr.width / 2.f,
                             zr.top + zr.height / 2.f);

        float dx = playerCenter.x - ghostCenter.x;
        float dy = playerCenter.y - ghostCenter.y;
        float dist2 = dx * dx + dy * dy;

        // make shield actually block damage
        if (dist2 < sumR2) {
            if (!g.shieldActive) {

                // Choose damage based on ghost type
                int ghostDamage = WHITE_GHOST_DAMAGE;

                int ghostType = g.ghosts[i].getType();
                switch (ghostType) {
                    case GHOST_GREEN:
                        ghostDamage = GREEN_GHOST_DAMAGE;
                        break;
                    case GHOST_BLUE:
                        ghostDamage = BLUE_GHOST_DAMAGE;
                        break;
                    case GHOST_RED:
                        ghostDamage = RED_GHOST_DAMAGE; // red hurts more
                        break;
                    case GHOST_WHITE:
                    default:
                        ghostDamage = WHITE_GHOST_DAMAGE;
                        break;
                }

                // Apply resistance
                int adjustedDamage =
                    static_cast<int>(std::round(ghostDamage * g.damageScale));
                if (adjustedDamage < 1)
                    adjustedDamage = 1;

                if (g.player.hit(g.gameTimeTotal, adjustedDamage)) {
                    g.hit.play();
                }

                if (g.player.getHealth() <= 0) {
                    g.state = State::GAME_OVER;
                    std::ofstream outputFile("gamedata/scores.txt");
                    outputFile << g.hiScore;
                    outputFile.close();
                }
            } else {
                // Debug: player was touched but shielded
                std::cout
                    << "[DEBUG] Ghost collision ignored (shield active)\n";
            }
        }

    } // End player touched

    //. Has the player touched health pickup
    if (g.player.getPosition().intersects(g.healthPickup.getPosition()) &&
        g.healthPickup.isSpawned()) {
        g.player.increaseHealthLevel(g.healthPickup.gotIt());
        // Play a sound
        g.pickup.play();
    }

    //. Has the player touched ammo pickup
    if (g.player.getPosition().intersects(g.ammoPickup.getPosition()) &&
        g.ammoPickup.isSpawned()) {
        g.bulletsSpare += g.ammoPickup.gotIt();
    }

    // Has the player touched SHIELD pickup (sprite)
    if (g.shieldPickupSpawned && g.player.getPosition().intersects(
                                     g.shieldPickupSprite.getGlobalBounds())) {

        g.shieldActive = true;
        g.shieldTimer = 5.f; // seconds of invincibility
        g.shieldPickupSpawned = false;
        g.powerup.play();

        std::cout << "[DEBUG] Shield pickup collected. "
                  << "Invincibility for " << g.shieldTimer << "s\n";
    }

    // Has the player touched FREEZE pickup (sprite)
    if (g.freezePickupSpawned && g.player.getPosition().intersects(
                                     g.freezePickupSprite.getGlobalBounds())) {

        g.freezeActive = true;
        g.freezeTimer = 4.f; // seconds of freeze
        g.freezePickupSpawned = false;
        g.powerup.play();

        std::cout << "[DEBUG] Freeze pickup collected. "
                  << "Freeze for " << g.freezeTimer << "s\n";
    }
}
// End of checkCollisions