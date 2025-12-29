#include <sstream>
#include <cstdlib>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Player1.hpp"
#include "Player2.hpp"
#include "Ball.hpp"

using namespace sf;

/*
****************************************
Global Variables
****************************************
*/

// --- Audio ---
Music bgMusic;

// --- Sound Effects ---
SoundBuffer barkBuffer;
Sound barkSound;
SoundBuffer snarlBuffer;
Sound snarlSound;

// --- Window ---
int winX = 1600;
int winY = 900;

// --- Scoring ---
int scoreLeft = 0;  // Zombie score
int scoreRight = 0; // Dog score
int scoreGoal = 10; // Winning condition (first to 10)
bool gameOver = false;
bool gameStarted = false;

// --- Ghost Dog Power-up ---
bool ghostActive = false;
float ghostTimer = 0.0f;

// --- Ball variables ---
bool ballVisible = false;
bool dogHolding = false;

/*
****************************************
HUD Helper Function
****************************************
*/
void centerMessage(Text &text, const std::string &str, int winX, int winY)
{
    text.setString(str);
    FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f,
                   bounds.top + bounds.height / 2.f);
    text.setPosition(winX / 2.f, winY / 2.f);
}

/*
****************************************
Main Game Function
****************************************
*/
int main()
{
    //---- Window Setup ----//
    VideoMode vm(winX, winY);
    RenderWindow window(vm, "Fetch?", Style::Default);

    // Limit framerate for stability
    window.setFramerateLimit(65);

    //---- Background ----//
    Texture textureTileset;
    if (!textureTileset.loadFromFile("graphics/spookytileset.png")) {
        std::cout << "Failed to load tileset!" << std::endl;
    }
    // 32x32 tile from row 1, col 18
    IntRect dirtTile(544, 0, 32, 32);
    Sprite dirtSprite(textureTileset, dirtTile);

    //------ Decorations ------//
    IntRect grave1(640, 384, 32, 32);
    IntRect grave2(672, 384, 32, 32);
    IntRect grave3(704, 384, 32, 32);
    Sprite graveSprite1(textureTileset, grave1);
    Sprite graveSprite2(textureTileset, grave2);
    Sprite graveSprite3(textureTileset, grave3);
    // Scale to 2x like the dirt
    graveSprite1.setScale(2.f, 2.f);
    graveSprite2.setScale(2.f, 2.f);
    graveSprite3.setScale(2.f, 2.f);
    graveSprite1.setPosition(winX / 3.f, winY - 96.f);     // bottom third
    graveSprite2.setPosition(winX / 2.f, winY - 96.f);     // center
    graveSprite3.setPosition(2 * winX / 3.f, winY - 96.f); // right third

    //------ Music ------//
    if (!bgMusic.openFromFile("sound/matt-picciano-game2-music.ogg")) {
        std::cout << "Failed to load music file!" << std::endl;
    }
    bgMusic.setLoop(true); // make it loop
    bgMusic.setVolume(30); // optional, 0–100
    bgMusic.play();

    //------ Font & Text ------//
    RectangleShape hudBackground;
    Text hud;
    Text scoreText;
    Text hudZombie;
    Text hudDog;
    Text messageText;
    Font font;
    font.loadFromFile("fonts/DeadFontWalking.otf");

    // HUD background panel
    hudBackground.setSize(Vector2f(400.f, 80.f));    // width, height
    hudBackground.setFillColor(Color(0, 0, 0, 150)); // semi-transparent black
    hudBackground.setOrigin(hudBackground.getSize().x / 2.f,
                            hudBackground.getSize().y / 2.f);
    hudBackground.setPosition(winX / 2.f, 60.f); // near top center

    // Format the HUD
    hud.setFont(font);
    hud.setCharacterSize(75);
    hud.setFillColor(Color::White);
    FloatRect hudBounds = hud.getLocalBounds();
    hud.setOrigin(hudBounds.left + hudBounds.width / 2.0f,
                  hudBounds.top + hudBounds.height / 2.0f);
    hud.setPosition(winX / 2.0f, 50.0f);

    // Format the score texts
    hudZombie.setFont(font);
    hudDog.setFont(font);
    hudZombie.setCharacterSize(48);
    hudDog.setCharacterSize(48);
    hudZombie.setFillColor(Color::White);
    hudDog.setFillColor(Color::White);
    hudZombie.setPosition(winX / 2.f - 150.f, 50.f);
    hudDog.setPosition(winX / 2.f + 150.f, 50.f);

    // Format the win/start message text
    messageText.setFont(font);
    messageText.setCharacterSize(75);
    messageText.setFillColor(Color::White);
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height / 2.0f);
    messageText.setPosition(winX / 2.0f, winY / 2.0f);

    // Assign the start message
    centerMessage(messageText,
                  "Press Enter to start\n\n"
                  "P1 Zombie: W/S move\n"
                  "P2 Dog: Up/Down move\n"
                  "        Spacebar to throw\n"
                  "Dog ability: P (Ghost Buddy)",
                  winX, winY);

    //------ Players & "Ball" ----//
    // Ball (zombie head)
    Texture textureBall;
    textureBall.loadFromFile("graphics/ball.png");
    Ball ball(winX / 2, 0, textureBall);

    // Zombie (Player1)
    Texture textureZombieFull;
    textureZombieFull.loadFromFile("graphics/zombie.png");
    Texture textureZombieHeadless;
    textureZombieHeadless.loadFromFile("graphics/zombie-headless.png");

    // get zombie width after scaling (half size)
    float zombieWidth = textureZombieFull.getSize().x * 0.5f;
    float zombieHeight = textureZombieFull.getSize().y * 0.5f;

    // Dog (Player2)
    Texture texturePlayer2;
    texturePlayer2.loadFromFile("graphics/dog.png");
    // Row 4, Col 5 on a 32x32 sheet: IntRect(128, 96, 32, 32)
    IntRect dogCell(128, 96, 32, 32);
    float dogHalfWidth =
        dogCell.width * 0.5f * 2.5f; // rect width * half * scale

    // Start as full zombie
    Player1 zombieLeft(zombieWidth / 2.f, winY / 2.f, Orientation::Vertical,
                       textureZombieFull);
    // Dog
    Player2 dogRight(winX - dogHalfWidth, winY / 2.f, Orientation::Vertical,
                     texturePlayer2, dogCell, 2.5f, 2.5f);
    // Ghost Dog helper (power-up)
    Player2 ghostDog(winX - dogHalfWidth, winY / 2.f, Orientation::Vertical,
                     texturePlayer2, dogCell, 2.5f, 2.5f);
    ghostDog.setColor(Color(255, 255, 255, 128));

    //------ Sound Effects ----//
    // Dog Bark
    if (!barkBuffer.loadFromFile("sound/dog-bark.wav")) {
        std::cout << "Failed to load bark sound!" << std::endl;
    }
    barkSound.setBuffer(barkBuffer);
    barkSound.setVolume(60); // optional (0–100)

    // Zombie Snarl
    if (!snarlBuffer.loadFromFile("sound/monster-snarl.wav")) {
        std::cout << "Failed to load snarl sound!" << std::endl;
    }
    snarlSound.setBuffer(snarlBuffer);
    snarlSound.setVolume(60); // optional (0–100)

    //------ Time ----//
    Clock clock;

    /*
    ****************************************
    Start the game loop
    ****************************************
    */
    while (window.isOpen()) {

        /*
        ****************************************
        Handle the players input
        ****************************************
        */

        Event event;

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                // Quit the game when the window is closed
                window.close();
        }

        // Handle the player quitting
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        // Controls for zombieLeft
        if (Keyboard::isKeyPressed(Keyboard::W))
            zombieLeft.moveNegative();
        else
            zombieLeft.stopNegative();
        if (Keyboard::isKeyPressed(Keyboard::S))
            zombieLeft.movePositive();
        else
            zombieLeft.stopPositive();

        // Controls for dogRight
        if (Keyboard::isKeyPressed(Keyboard::Up))
            dogRight.moveNegative();
        else
            dogRight.stopNegative();
        if (Keyboard::isKeyPressed(Keyboard::Down))
            dogRight.movePositive();
        else
            dogRight.stopPositive();

        // Activate Ghost Dog with P
        if (Keyboard::isKeyPressed(Keyboard::P) && !ghostActive) {
            ghostActive = true;
            ghostTimer = 3.0f; // lasts 3 seconds
        }

        // Catch / throw
        bool spacePressed = Keyboard::isKeyPressed(Keyboard::Space);

        // Space key for Dog to toss the ball
        if (Keyboard::isKeyPressed(Keyboard::Space) && dogHolding) {
            dogHolding = false;

            // Launch ball leftwards from dog’s mouth
            FloatRect dogPos = dogRight.getPosition();
            float dogCenterY = dogPos.top + dogPos.height / 2.f;

            ball.reset(dogPos.left - 50, dogCenterY);
            ball.setDirection(-0.3f, 0.f);  // Force velocity left (towardZombie)
        }

        /*
        ****************************************
        Update the scene
        ****************************************
        */

        // Wait on game start
        if (!gameStarted) {

            // Show "Press Enter to start!" until Enter is pressed
            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                gameStarted = true;
                gameOver = false;
                scoreLeft = 0;
                scoreRight = 0;

                // Switch zombie sprite to headless
                zombieLeft.setTexture(textureZombieHeadless);

                // Now reveal the "ball"
                ballVisible = true;

                // Reset the ball near the Zombie to start play
                ball.reset(winX / 4, winY / 2);

                // Clear the message
                centerMessage(messageText, "", winX, winY);
            }
        }
        // Game over check
        else if (gameOver) {
            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                // Restart round
                gameOver = false;
                scoreLeft = 0;
                scoreRight = 0;

                // Hide ball until Enter is pressed
                ballVisible = true;
                ball.reset(winX / 4, winY / 2);

                zombieLeft.setTexture(textureZombieHeadless);
                centerMessage(messageText, "", winX, winY);
            }
        }
        // Play until goal reached
        else {

            // Update the delta time
            Time dt = clock.restart();

            // Player updates
            zombieLeft.update(dt, winX, winY);
            dogRight.update(dt, winX, winY);
            // Dog buddy check & update
            if (ghostActive) {
                ghostTimer -= dt.asSeconds();

                FloatRect dogPos = dogRight.getPosition();
                float offset = 100.0f;

                // follow Dog’s Y with offset
                float dogCenterY = dogPos.top + dogPos.height / 2.f;
                ghostDog.setY(dogCenterY + offset);

                if (ghostTimer <= 0) {
                    ghostActive = false;
                }
            }

            // Ball update
            ball.update(dt, winX, winY);

            // Dog collision
            if (ball.getPosition().intersects(dogRight.getPosition()) &&
                !dogHolding) {
                if (!ball.hasJustHit()) {
                    // Instead of bouncing, Dog catches the ball
                    dogHolding = true;
                    ballVisible = true;
                    ball.setJustHit(true);
                    scoreRight++;
                }
            }
            // Ghost Dog collision (if active)
            else if (ghostActive &&
                     ball.getPosition().intersects(ghostDog.getPosition())) {
                if (!ball.hasJustHit()) {
                    ball.reboundSides();
                    scoreRight++;
                    ball.setJustHit(true);
                    // Left out ghost bark on purpose for now
                }
            }
            // Zombie collision
            else if (ball.getPosition().intersects(zombieLeft.getPosition())) {
                if (!ball.hasJustHit()) {
                    ball.reboundSides();
                    ball.setJustHit(true);

                    // Snarl
                    snarlSound.play();

                    // Trigger Head Spin
                    ball.activateHeadSpin(3.0f, 1.5f); // 3 seconds, 1.5x faster
                }
            } else {
                // Ball is not colliding with either bat
                ball.setJustHit(false);
            }

            // Dog is holding the ball?
            if (dogHolding) {
                FloatRect dogPos = dogRight.getPosition();

                // Approximate mouth position with offsets
                float mouthX = dogPos.left - 10.f;    // slightly left of dog
                float mouthY = dogPos.top + (dogPos.height / 2.f) - 20.f; //above center

                ball.reset(mouthX, mouthY);
            }          

            // If Dog hits Space while NOT holding -> try to catch
            if (spacePressed && !dogHolding &&
                ball.getPosition().intersects(dogRight.getPosition()) &&
                !ball.hasJustHit()) {

                dogHolding = true;
                ballVisible = true; // keep visible in mouth
                ball.setJustHit(true);
                scoreRight++;     // award point for catch
                barkSound.play(); // bark on success
            }

            // If Dog hits Space while holding -> throw
            else if (spacePressed && dogHolding) {
                dogHolding = false;

                FloatRect dogPos = dogRight.getPosition();
                float mouthX = dogPos.left - 10.f;
                float mouthY = dogPos.top + (dogPos.height / 2.f) - 20.f;

                ball.reset(mouthX, mouthY);
                ball.setDirection(-0.3f, 0.f); // throw leftwards
            }

            // Ball went off the right side: Zombie scores
            if (ball.getPosition().left + ball.getPosition().width > winX) {
                scoreLeft++;
                ball.reset(winX / 2, winY / 2);
            }

            // Ball went off the left side: could just reset without scoring
            if (ball.getPosition().left < 0) {
                ball.reset(winX / 2, winY / 2);
            }

            // Update HUD text
            std::stringstream ssZombie, ssDog;
            ssZombie << "Zombie: " << scoreLeft;
            ssDog << "Dog: " << scoreRight;

            hudZombie.setString(ssZombie.str());
            hudDog.setString(ssDog.str());

            // Measure text bounds (after updating strings)
            FloatRect zBounds = hudZombie.getLocalBounds();
            FloatRect dBounds = hudDog.getLocalBounds();

            float spacing = 100.f; // gap between texts
            float totalWidth = zBounds.width + dBounds.width + spacing;
            float panelHeight = std::max(zBounds.height, dBounds.height) + 40.f;

            // Resize & center HUD background
            hudBackground.setSize(Vector2f(totalWidth + 60.f, panelHeight));
            hudBackground.setOrigin(hudBackground.getSize().x / 2.f,
                                    hudBackground.getSize().y / 2.f);
            hudBackground.setPosition(winX / 2.f, 60.f);

            // Center text origins
            hudZombie.setOrigin(zBounds.left + zBounds.width / 2.0f,
                                zBounds.top + zBounds.height / 2.0f);
            hudDog.setOrigin(dBounds.left + dBounds.width / 2.0f,
                             dBounds.top + dBounds.height / 2.0f);

            // Place texts symmetrically around center
            hudZombie.setPosition(winX / 2.f - totalWidth / 4.f, 60.f);
            hudDog.setPosition(winX / 2.f + totalWidth / 4.f, 60.f);

            // Win the game
            if (scoreLeft >= scoreGoal) {
                centerMessage(messageText, "Zombie Wins!", winX, winY);
                gameOver = true;
            } else if (scoreRight >= scoreGoal) {
                centerMessage(messageText, "Dog Wins!", winX, winY);
                gameOver = true;
            }
        }

        /*
        ****************************************
        Draw the scene
        ****************************************
        */

        // Clear the window
        window.clear();

        // Background is first
        // Tile the dirt across the window
        for (int x = 0; x < winX; x += 32) {
            for (int y = 0; y < winY; y += 32) {
                dirtSprite.setPosition(static_cast<float>(x),
                                       static_cast<float>(y));
                window.draw(dirtSprite);
            }
        }

        // Decorations (graves in front of dirt)
        window.draw(graveSprite1);
        window.draw(graveSprite2);
        window.draw(graveSprite3);

        // HUD elements, in order
        window.draw(hudBackground);
        window.draw(hud);
        window.draw(hudZombie);
        window.draw(hudDog);

        // Players
        zombieLeft.draw(window);
        dogRight.draw(window);
        if (ghostActive)
            ghostDog.draw(window);

        // Ball (starts invisible until zombie "creates it")
        if (ballVisible) {
            ball.draw(window);
        }

        // Draw start or winner message if needed
        if (!gameStarted || gameOver) {
            window.draw(messageText);
        }

        // Display
        window.display();

    } // End of game loop

    return 0;
} // End of main