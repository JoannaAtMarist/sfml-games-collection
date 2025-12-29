#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

// Function declaration: Kitty animation
void updatePlayerFrame(Sprite &player, int &playerFrame, float &frameTimer,
                       float frameDuration);

// Function declaration: Cauldron animation
void updateCauldronSprite(sf::Sprite &sprite, sf::IntRect cauldronRects[8][12],
                          float progress, int frame);

/*
****************************************
Globals
****************************************
*/

/*------ Player ------*/
// Player aka Witch Cat
int playerFrame = 0;               // current animation frame (0–5)
float frameDuration = 0.15f;       // time per frame in seconds
float frameTimer = 0.0f;           // counts elapsed time
const float PLAYER_SPEED = 400.0f; // pixels per second

bool isHopping = false;
float hopTimer = 0.0f;
const float HOP_DURATION = 0.5f; // seconds of hop (up + down)
const float HOP_HEIGHT = 250.f;  // pixels up from baseline

/*------ Dangers ------*/
// Bat
int batFrame = 0;
float batFrameDuration = 0.1f; // faster flap than cat
float batFrameTimer = 0.0f;

// Ghost
Texture textureGhost;
Sprite spriteGhost;
int ghostFrame = 0;
float ghostFrameDuration = 0.2f; // speed of flap
float ghostFrameTimer = 0.0f;
// Ghost state
bool ghostActive = false;
float ghostSpeed = 150.f; // pixels per second (downward)

/*------ Collectables ------*/
// Pumpkins
const int NUM_COLLECTABLES = 9; // how many pumpkins at once
Texture textureCollectable;
Sprite collectables[NUM_COLLECTABLES];
bool collectableActive[NUM_COLLECTABLES];
float collectableSpawnTimer[NUM_COLLECTABLES];
float collectableSpawnInterval = 1.0f; // seconds between spawns
float collectableFallSpeed = 250.f;    // pixels per second

/*------ Score ------*/
int scoreGoal = 50;

/*
****************************************
main
****************************************
*/
int main()
{
    // Create a video mode object
    VideoMode vm(1920, 1080);

    // Create and open a window for the game
    RenderWindow window(vm, "Witch Cat", Style::Default);

    // Limit framerate for stability
    window.setFramerateLimit(65);

    /*---- Background ----*/
    Texture textureBackground;
    textureBackground.loadFromFile("graphics/bg.png");
    Sprite spriteBackground;
    spriteBackground.setTexture(textureBackground);
    spriteBackground.setPosition(0, 0);
    spriteBackground.setScale(1920.f / textureBackground.getSize().x,
                              1080.f / textureBackground.getSize().y);

    /*------ Decorations ------*/
    // make 3 cloud sprites from 1 texture
    Texture textureCloud;
    // Load 1 new texture
    textureCloud.loadFromFile("graphics/cloud.png");
    // Make the clouds with arrays
    const int NUM_CLOUDS = 6;
    Sprite clouds[NUM_CLOUDS];
    int cloudSpeeds[NUM_CLOUDS];
    bool cloudsActive[NUM_CLOUDS];
    for (int i = 0; i < NUM_CLOUDS; i++) {
        clouds[i].setTexture(textureCloud);
        clouds[i].setPosition(-300, i * 150);
        cloudsActive[i] = false;
        cloudSpeeds[i] = 0;
    }

    /*------ Player ------*/
    // Prepare the player
    Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player.png");
    Sprite spritePlayer;
    spritePlayer.setTexture(texturePlayer);

    // Each frame is 48x64, so set origin to the middle
    // (24 = half width, 32 = half height)
    spritePlayer.setOrigin(24, 32);
    spritePlayer.setScale(2.f, 2.f);

    // Animate adorable witch
    // Show first frame (frame 0)
    spritePlayer.setTextureRect(IntRect(0, 0, 48, 48));

    // Start on the left side of the screen
    spritePlayer.setPosition(580, 700);

    /*------ Dangers ------*/
    // Bat is lethal & Ghost subtracts from score

    // Prepare the bat
    Texture textureBat;
    textureBat.loadFromFile("graphics/bat.png");
    Sprite spriteBat;
    spriteBat.setTexture(textureBat);

    // Bat sprite math
    const int BAT_FRAME_W = 201;  // 3×67
    const int BAT_FRAME_H = 201;  // 3×67
    const int BAT_NUM_FRAMES = 8; // 8 frames across

    // First frame
    spriteBat.setTextureRect(IntRect(0, 0, BAT_FRAME_W, BAT_FRAME_H));
    spriteBat.setOrigin(BAT_FRAME_W / 2, BAT_FRAME_H / 2);
    spriteBat.setScale(0.75f, 0.75f);

    // Is the bat currently moving?
    bool batActive = false;
    // How fast can the bat fly
    float batSpeed = 0.0f;

    // Bat sound
    SoundBuffer batBuffer;
    batBuffer.loadFromFile("sound/bat_01.ogg");
    Sound batSound;
    batSound.setBuffer(batBuffer);

    // Prepare the ghost
    textureGhost.loadFromFile("graphics/ghost.png");
    spriteGhost.setTexture(textureGhost);

    // Ghost sprite math
    const int GHOST_FRAME_W = 52;   // 4 cols × 13 px
    const int GHOST_FRAME_H = 65;   // 5 rows × 13 px
    const int GHOST_NUM_FRAMES = 8; // number of frames across sheet

    // Ghost animation
    int ghostFrame = 0;
    float ghostFrameTimer = 0.f;
    float ghostFrameDuration = 0.2f;

    spriteGhost.setTexture(textureGhost);
    spriteGhost.setTextureRect(IntRect(0, 0, GHOST_FRAME_W, GHOST_FRAME_H));
    spriteGhost.setOrigin(GHOST_FRAME_W / 2, GHOST_FRAME_H / 2);
    spriteGhost.setScale(1.5f, 1.5f);

    // Ghost sound
    SoundBuffer ghostBuffer;
    ghostBuffer.loadFromFile("sound/mushroom.ogg");
    Sound ghostSound;
    ghostSound.setBuffer(ghostBuffer);

    /*------ Collectables ------*/
    // Prepare the collectable pumpkin
    textureCollectable.loadFromFile("graphics/pumpkin3.png");
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        collectables[i].setTexture(textureCollectable);
        collectables[i].setScale(3.f, 3.f);
        collectables[i].setOrigin(collectables[i].getLocalBounds().width / 2.f,
                                  collectables[i].getLocalBounds().height /
                                      2.f);
        collectables[i].setPosition(-2000, -2000); // hidden offscreen
        collectableActive[i] = false;
        collectableSpawnTimer[i] = 0.f;
    }

    /*---- Cauldron ----*/
    // Load the cauldron sprite sheet
    Texture textureCauldron;
    textureCauldron.loadFromFile("graphics/cauldron.png");

    Sprite spriteCauldron;
    spriteCauldron.setTexture(textureCauldron);

    // Dimensions (each cauldron is 4x4 grid squares of 12px each = 48px)
    const int CAULDRON_SIZE = 48;
    const int NUM_COLS = 12;
    const int NUM_ROWS = 8;

    // Build a grid of IntRects for every cauldron
    sf::IntRect cauldronRects[NUM_ROWS][NUM_COLS];
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            cauldronRects[row][col] =
                sf::IntRect(col * CAULDRON_SIZE, row * CAULDRON_SIZE,
                            CAULDRON_SIZE, CAULDRON_SIZE);
        }
    }

    // Animation controls
    int cauldronFrame = 0;
    float cauldronFrameDuration = 0.2f;
    float cauldronFrameTimer = 0.0f;

    // Initial frame -> top-right cauldron (row 0, col 11)
    spriteCauldron.setTextureRect(cauldronRects[0][11]);
    spriteCauldron.setOrigin(CAULDRON_SIZE / 2.f, CAULDRON_SIZE / 2.f);
    spriteCauldron.setPosition(960, 720);
    spriteCauldron.setScale(3.f, 3.f); // scale up THRICE for visibility

    /*------ Score ------*/
    int score = 0;

    /*------ Font & Text ------*/
    Text messageText;
    Text scoreText;
    Text fpsText;

    // We need to choose a font
    Font font;
    font.loadFromFile("fonts/HappyHalloween.ttf");

    // Set the font to our message
    messageText.setFont(font);
    scoreText.setFont(font);

    // Assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

    // Make it really big, not
    messageText.setCharacterSize(63);
    scoreText.setCharacterSize(63);

    // Choose a color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    // Position the text
    FloatRect textRect = messageText.getLocalBounds();

    messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height / 2.0f);
    messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
    scoreText.setPosition(20, 20);

    // Backgrounds for the text
    RectangleShape rect1;
    rect1.setFillColor(sf::Color(0, 0, 0, 150));
    rect1.setSize(Vector2f(600, 80));
    rect1.setPosition(0, 23);

    /*------ Time ------*/
    Clock clock;

    // Ask the window for its current size
    Vector2u winSize = window.getSize();

    // Time bar
    RectangleShape timeBar;
    float timeBarStartWidth = winSize.x * 0.5f; // half the window width
    float timeBarHeight = 40.f;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color(20, 80, 0));
    // centered horizontally, 20px above bottom
    timeBar.setPosition((winSize.x / 2.f) - (timeBarStartWidth / 2.f),
                        winSize.y - timeBarHeight - 20.f);

    float timeRemaining = 10.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    // Track whether the game is running
    bool paused = true;

    /*------ Death ------*/
    // Prepare the gravestone
    const int DEATH_FRAME_W = 32;
    const int DEATH_FRAME_H = 32;

    Texture textureRIP;
    textureRIP.loadFromFile("graphics/box.png");
    Sprite spriteRIP;
    spriteRIP.setTexture(textureRIP);
    spriteRIP.setPosition(580, 700);

    // Far right frame (index 11)
    spriteRIP.setTextureRect(
        IntRect(11 * DEATH_FRAME_W, 0, DEATH_FRAME_W, DEATH_FRAME_H));
    spriteRIP.setOrigin(DEATH_FRAME_W / 2, DEATH_FRAME_H / 2);
    spriteRIP.setScale(3.f, 3.f); // make it visible

    // Control the player input
    bool acceptInput = false;

    // Prepare the sound
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/meow.ogg");
    Sound death;
    death.setBuffer(deathBuffer);

    // Out of time
    SoundBuffer ootBuffer;
    ootBuffer.loadFromFile("sound/gameover.wav");
    Sound outOfTime;
    outOfTime.setBuffer(ootBuffer);

    // control the drawing of the score
    int lastDrawn = 0;

    /*
    ****************************************
    Start the game loop
    ****************************************
    */
    // Start the game loop
    while (window.isOpen()) {
        // score ++;
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::KeyReleased && !paused) {
                // Listen for key presses again
                acceptInput = true;

                // Kitty hop with spacebar
                if (!isHopping && event.key.code == Keyboard::Space) {
                    isHopping = true;
                    hopTimer = 0.0f;
                }
            }
        }

        /*
        ****************************************
        Handle the players input
        ****************************************
        */

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        // Start the game
        if (Keyboard::isKeyPressed(Keyboard::Return)) {
            paused = false;

            // Reset the time and the score
            score = 0;
            timeRemaining = 10.0f; // match to start time
            // Recalculate scaling for the bar
            timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

            // Make sure the gravestone is hidden
            spriteRIP.setPosition(675, 2000);

            // Move the player into position
            spritePlayer.setPosition(580, 700);

            acceptInput = true;
        }

        /*
        ****************************************
        Update the scene
        ****************************************
        */
        if (!paused) {

            // Measure time
            Time dt = clock.restart();

            // =======================================
            // Player movement
            // =======================================

            // Animate Witch Cat
            frameTimer += dt.asSeconds();
            if (frameTimer >= frameDuration) {
                updatePlayerFrame(spritePlayer, playerFrame, frameTimer,
                                  frameDuration);
            }

            // Left/right movement only
            if (!paused) {
                if (Keyboard::isKeyPressed(Keyboard::Left)) {
                    spritePlayer.move(-PLAYER_SPEED * dt.asSeconds(), 0);
                    spritePlayer.setScale(2.f, 2.f); // face left
                }
                if (Keyboard::isKeyPressed(Keyboard::Right)) {
                    spritePlayer.move(PLAYER_SPEED * dt.asSeconds(), 0);
                    spritePlayer.setScale(-2.f, 2.f); // face right
                }
            }

            // Kitty hop with spacebar
            const float baselineY = 700.f;

            if (isHopping) {
                hopTimer += dt.asSeconds();
                if (hopTimer < HOP_DURATION / 2) {
                    // going up
                    float t = hopTimer / (HOP_DURATION / 2); // goes 0 to 1
                    spritePlayer.setPosition(spritePlayer.getPosition().x,
                                             baselineY - (HOP_HEIGHT * t));
                } else if (hopTimer < HOP_DURATION) {
                    // going down
                    float t2 =
                        (hopTimer - (HOP_DURATION / 2)) / (HOP_DURATION / 2);
                    spritePlayer.setPosition(spritePlayer.getPosition().x,
                                             baselineY -
                                                 (HOP_HEIGHT * (1 - t2)));
                } else {
                    // hop finished
                    isHopping = false;
                    hopTimer = 0.0f;
                    spritePlayer.setPosition(spritePlayer.getPosition().x,
                                             baselineY);
                }
            }

            // Clamp kitty inside window bounds
            Vector2f pos = spritePlayer.getPosition();
            if (pos.x < 0)
                pos.x = 0;
            if (pos.x > 1920)
                pos.x = 1920;
            if (!isHopping)
                pos.y = baselineY;
            spritePlayer.setPosition(pos);

            // =======================================
            // Cauldron
            // =======================================

            // Animate bubbling
            cauldronFrameTimer += dt.asSeconds();
            if (cauldronFrameTimer >= cauldronFrameDuration) {
                cauldronFrameTimer = 0.0f;
                cauldronFrame = (cauldronFrame + 1) % 6; // cycle 0–5
            }
            float progress = static_cast<float>(score) / scoreGoal;
            updateCauldronSprite(spriteCauldron, cauldronRects, progress,
                                 cauldronFrame);

            // Subtract from the amount of time remaining
            timeRemaining -= dt.asSeconds();
            // size up the time bar
            timeBar.setSize(
                Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

            if (timeRemaining <= 0.0f) {
                // Pause the game
                paused = true;

                // Change the message shown to the player
                messageText.setString("Out of time!!");

                // Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);

                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

                // Play the out of time sound
                outOfTime.play();
            }

            // Win the game: disabled
            /*
            if (score >= scoreGoal && !paused) {
                paused = true;
                messageText.setString("You filled the cauldron! You Win!");

                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);
                messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
            }
            */

            /*----- Dangers -----*/
            // Setup the bat
            if (!batActive) {
                // How fast is the bat
                srand((int)time(0) * 10);
                batSpeed = (rand() % 700) + 400; // 400–1100 px/sec

                // How high is the bat
                srand((int)time(0) * 10);
                float height =
                    (rand() % 900) + 100; // spawns anywhere Y=100–999
                spriteBat.setPosition(2000, height);
                batActive = true;

                // Play bat sound when it spawns
                batSound.play();
            } else {
                // Animate bat wings
                batFrameTimer += dt.asSeconds();
                if (batFrameTimer >= batFrameDuration) {
                    batFrameTimer = 0.f;
                    batFrame = (batFrame + 1) % BAT_NUM_FRAMES;
                    spriteBat.setTextureRect(IntRect(batFrame * BAT_FRAME_W, 0,
                                                     BAT_FRAME_W, BAT_FRAME_H));
                }

                // Move the bat leftward
                spriteBat.setPosition(spriteBat.getPosition().x -
                                          (batSpeed * dt.asSeconds()),
                                      spriteBat.getPosition().y);

                // Reset once off screen
                if (spriteBat.getPosition().x < -300) {
                    batActive = false;
                }
            }
            // Bat collision with player (allows for sprite blank space)
            FloatRect batBounds = spriteBat.getGlobalBounds();

            // shrink hitbox for fairness
            batBounds.left += 40;
            batBounds.width -= 80;
            batBounds.top += 30;
            batBounds.height -= 60;

            if (spritePlayer.getGlobalBounds().intersects(batBounds)) {

                paused = true;

                // Push bat off screen
                spriteBat.setPosition(-400, spriteBat.getPosition().y);
                batActive = false;

                // Message
                messageText.setString("The bat got you!");
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);
                messageText.setPosition(1920 / 2.0f, 200);

                // Play death sound
                death.play();

                // Move gravestone into view
                spriteRIP.setPosition(spritePlayer.getPosition().x,
                                      spritePlayer.getPosition().y);

                // Hide kitty when dead
                spritePlayer.setPosition(-2000, -2000);
            }

            // Ghost logic
            if (!ghostActive) {
                // spawn at random X, off top
                if (rand() % 200 == 0) { // ~1 in 200 chance per frame
                    float x = (rand() % 1600) + 160;
                    spriteGhost.setPosition(x, -50);
                    ghostActive = true;
                    ghostFrame = 0;
                    ghostFrameTimer = 0.0f;
                    // first frame
                    spriteGhost.setTextureRect(
                        IntRect(0, 0, GHOST_FRAME_W, GHOST_FRAME_H));
                }
            } else {
                // Animate ghost
                ghostFrameTimer += dt.asSeconds();
                if (ghostFrameTimer >= ghostFrameDuration) {
                    ghostFrameTimer = 0.f;
                    ghostFrame = (ghostFrame + 1) % GHOST_NUM_FRAMES;
                    spriteGhost.setTextureRect(
                        IntRect(ghostFrame * GHOST_FRAME_W, // x = frame index
                                0,                          // y = row
                                GHOST_FRAME_W, GHOST_FRAME_H));
                }

                // Move downward
                spriteGhost.move(0.f, ghostSpeed * dt.asSeconds());

                // Reset if off screen
                if (spriteGhost.getPosition().y > 1200) {
                    ghostActive = false;
                    spriteGhost.setPosition(-2000, -2000);
                }
            }
            // Ghost collision with player
            if (ghostActive && spritePlayer.getGlobalBounds().intersects(
                                   spriteGhost.getGlobalBounds())) {
                ghostActive = false;
                spriteGhost.setPosition(-2000, -2000);

                // Penalty: lose score
                score -= 5;
                if (score < 0)
                    score = 0;

                // Message
                messageText.setString("The ghost ate your candy!");
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                                      textRect.top + textRect.height / 2.0f);
                messageText.setPosition(1920 / 2.0f, 200);

                // Play ghost sound
                ghostSound.play();
            }

            /*----- Collectables -----*/
            for (int i = 0; i < NUM_COLLECTABLES; i++) {
                if (!collectableActive[i]) {
                    collectableSpawnTimer[i] += dt.asSeconds();
                    if (collectableSpawnTimer[i] >= collectableSpawnInterval) {
                        collectableSpawnTimer[i] = 0.f;
                        collectableActive[i] = true;

                        // Pick random X across screen
                        float x = (rand() % 1600) + 160; // avoid edges
                        collectables[i].setPosition(
                            x, -50); // start just above top
                    }
                } else {
                    // Move downward
                    collectables[i].move(0.f,
                                         collectableFallSpeed * dt.asSeconds());

                    // Reset if off screen
                    if (collectables[i].getPosition().y > 1200) {
                        collectableActive[i] = false;
                        collectables[i].setPosition(-2000, -2000);
                    }

                    // Collision with player
                    if (spritePlayer.getGlobalBounds().intersects(
                            collectables[i].getGlobalBounds())) {
                        collectableActive[i] = false;
                        collectables[i].setPosition(-2000, -2000);

                        // reward: increase score or time
                        score += 10;           // add points
                        timeRemaining += 1.0f; // give bonus second

                        // Update cauldron fill based on progress
                        float progress = static_cast<float>(score) / scoreGoal;
                        updateCauldronSprite(spriteCauldron, cauldronRects,
                                             progress, cauldronFrame);
                    }
                }
            }

            /*----- Decoration -----*/
            // Manage the clouds with arrays
            for (int i = 0; i < NUM_CLOUDS; i++) {
                if (!cloudsActive[i]) {
                    // How fast is the cloud
                    srand((int)time(0) * i);
                    cloudSpeeds[i] = (rand() % 200);

                    // How high is the cloud
                    srand((int)time(0) * i);
                    float height = (rand() % 150);
                    clouds[i].setPosition(-200, height);
                    cloudsActive[i] = true;

                } else {
                    // Set the new position
                    clouds[i].setPosition(clouds[i].getPosition().x +
                                              (cloudSpeeds[i] * dt.asSeconds()),
                                          clouds[i].getPosition().y);

                    // Has the cloud reached the right hand edge of the screen?
                    if (clouds[i].getPosition().x > 1920) {
                        // Set it up ready to be a whole new cloud next frame
                        cloudsActive[i] = false;
                    }
                }
            }

            // Draw the score and the frame rate once every 100 frames
            lastDrawn++;
            if (lastDrawn == 100) {
                // Update the score text
                std::stringstream ss;
                ss << "Score = " << score;
                scoreText.setString(ss.str());

                // Draw the fps
                std::stringstream ss2;
                ss2 << "FPS = " << 1 / dt.asSeconds();
                fpsText.setString(ss2.str());
                lastDrawn = 0;
            }
        }
        // End of Update the scene

        /*
        ****************************************
        Draw the scene
        ****************************************
        */

        // Clear everything from the last frame
        window.clear();

        // Draw our game scene here
        window.draw(spriteBackground);

        // Draw the clouds
        for (int i = 0; i < NUM_CLOUDS; i++) {
            window.draw(clouds[i]);
        }

        // Draw the Cauldron
        window.draw(spriteCauldron);

        // Draw the player
        window.draw(spritePlayer);

        // Draw the gravestone
        window.draw(spriteRIP);

        // Draw background for the text
        window.draw(rect1);

        // Draw the bat
        window.draw(spriteBat);
        // Draw the ghost
        if (ghostActive)
            window.draw(spriteGhost);

        // Draw the collectable
        for (int i = 0; i < NUM_COLLECTABLES; i++) {
            if (collectableActive[i])
                window.draw(collectables[i]);
        }

        // Draw the score
        window.draw(scoreText);

        // Draw the FPS
        window.draw(fpsText);

        // Draw the timebar
        window.draw(timeBar);

        if (paused) {
            // Draw our message
            window.draw(messageText);
        }

        // Show everything we just drew
        window.display();
    }
    // End of game loop

    return 0;
} // End of main

// Function definition: Kitty animation
void updatePlayerFrame(sf::Sprite &player, int &playerFrame, float &frameTimer,
                       float frameDuration)
{
    frameTimer = 0.0f;
    playerFrame = (playerFrame + 1) % 6; // 6 frames across
    player.setTextureRect(IntRect(playerFrame * 48, 0, 48, 64));
}

/*----- Cauldron -----*/
// Function definition: Pick a cauldron frame based on score progress
void updateCauldronSprite(sf::Sprite &sprite, sf::IntRect cauldronRects[8][12],
                          float progress,
                          int frame) // bubbling frame
{
    // Clamp 0–1
    if (progress < 0.f)
        progress = 0.f;
    if (progress > 1.f)
        progress = 1.f;

    // Figure out which stage (0–3)
    int stage = static_cast<int>(progress * 4); // 0,1,2,3
    if (stage > 3)
        stage = 3;

    // Pick which row/col block to use
    int row, startCol;
    switch (stage) {
        case 0: // green bubbly
            row = 0;
            startCol = 6; // green starts at col 6
            break;
        case 1: // green smoky
            row = 1;
            startCol = 6;
            break;
        case 2: // purple bubbly
            row = 6;
            startCol = 6; // purple starts at col 6
            break;
        case 3: // purple smoky
            row = 7;
            startCol = 6;
            break;
    }

    // Cycle inside this 6-column block
    int col = startCol + (frame % 6);
    sprite.setTextureRect(cauldronRects[row][col]);
}
