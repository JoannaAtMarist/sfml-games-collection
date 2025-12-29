#include <random>
#include "LevelUpdate.hpp"
#include "PlatformUpdate.hpp"
#include "PlayerUpdate.hpp"
#include "SoundEngine.hpp"

// Implementation of LevelUpdate methods
void LevelUpdate::setWindowSize(sf::Vector2u size)
{
    m_ScreenSize = size;
}

// Assemble method to connect to player and level
void LevelUpdate::assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                           std::shared_ptr<PlayerUpdate> playerUpdate)
{
    m_PlayerPosition = playerUpdate->getPositionPointer();
    m_Player = playerUpdate.get(); // Save the pointer
}

// Connect to camera time pointer
void LevelUpdate::connectToCameraTime(float *cameraTime)
{
    m_CameraTime = cameraTime;
}

// Get pointer to paused state
bool *LevelUpdate::getIsPausedPointer()
{
    return &m_IsPaused;
}

// Generate a random number between minHeight and maxHeight
int LevelUpdate::getRandomNumber(int minHeight, int maxHeight)
{
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(minHeight, maxHeight);
    return dist(gen);
}

// Add a platform to the level
void LevelUpdate::addPlatform(PlatformUpdate *platform)
{
    m_Platforms.push_back(platform);
    m_NumberOfPlatforms++;
}

// Position the level at the start
void LevelUpdate::positionLevelAtStart()
{
    // Reset the start offset to 0
    // so we always start at the beginning of the world
    float startOffset = 0.f;

    // SCALING LOGIC:
    // If we haven't set the screen size yet, default to a safe value.
    float screenHeight = (m_ScreenSize.y > 0) ? (float)m_ScreenSize.y : 800.f;

    // Start slightly lower to give room for upward jumps
    float startY = screenHeight * 0.45f;

    //. PLATFORM 0 (Safe Start)
    // Access the rect via getPositionPointer() now
    m_Platforms[0]->getPositionPointer()->left = startOffset;
    m_Platforms[0]->getPositionPointer()->top = startY;
    m_Platforms[0]->getPositionPointer()->width = 250;
    m_Platforms[0]->getPositionPointer()->height = 64;

    // Make sure it's active!
    m_Platforms[0]->reset();

    // PLATFORMS 1 to END (Randomized)
    for (int i = 1; i < m_NumberOfPlatforms; ++i) {

        // Use m_Platforms[i-1]->getPositionPointer() to get previous values
        float currentY = m_Platforms[i - 1]->getPositionPointer()->top;
        float nextY = currentY + getRandomNumber(-80, 80);

        // Clamp height so they don't go off-screen
        float minY = screenHeight * 0.25f; // Ceiling
        float maxY = screenHeight * 0.65f; // Floor
        if (nextY < minY)
            nextY = minY;
        if (nextY > maxY)
            nextY = maxY;

        m_Platforms[i]->getPositionPointer()->top = nextY;

        // Gap logic
        float gap = getRandomNumber(30, 80);
        float prevLeft = m_Platforms[i - 1]->getPositionPointer()->left;
        float prevWidth = m_Platforms[i - 1]->getPositionPointer()->width;

        m_Platforms[i]->getPositionPointer()->left = prevLeft + prevWidth + gap;
        m_Platforms[i]->getPositionPointer()->width = getRandomNumber(120, 400);
        m_Platforms[i]->getPositionPointer()->height = 64;

        // Reset this platform too
        m_Platforms[i]->reset();
    }

    //. PLAYER PLACEMENT
    // Spawn on the very first platform (Left Edge)
    int startPlatIndex = 0;

    m_PlayerPosition->left =
        m_Platforms[startPlatIndex]->getPositionPointer()->left + 20;

    // Spawn high enough (-150) to definitely fall ONTO the platform
    m_PlayerPosition->top =
        m_Platforms[startPlatIndex]->getPositionPointer()->top - 110;

    // Reset Camera Time so the shader/background logic doesn't get weird
    if (m_CameraTime)
        *m_CameraTime = 0;

    // Reset generation pointers
    m_MoveRelativeToPlatform = m_NumberOfPlatforms - 1;
    m_NextPlatformToMove = 0;
}

// Update method called each frame
void LevelUpdate::update(float timeSinceLastUpdate)
{
    if (!m_IsPaused) {

        //. Check if player crossed the finish line
        if (m_PlayerPosition->left > LEVEL_LENGTH && !m_LevelComplete) {

            std::cout << "FINISH LINE CROSSED!" << std::endl; // Debug print
            m_LevelComplete = true;

            if (m_CurrentLevel == 1) {
                std::cout << "STARTING LEVEL 2..." << std::endl;
                m_CurrentLevel = 2;

                // Switch Music
                SoundEngine::playLevelMusic(2);

                // Reset everything to the start
                positionLevelAtStart();

                // Reset the trigger so we can win Level 2 later
                m_LevelComplete = false;

                // IMPORTANT: Return now so we don't run Game Over logic
                // on the reset frame (which might falsely trigger).
                return;

            } else if (m_CurrentLevel == 2) {
                std::cout << "VICTORY!" << std::endl;

                // Do NOT reset level to 1 here. Just flag it.
                m_GameWon = true;
                m_IsPaused = true;
                return;
            }
        }

        //. Game Over Logic
        if (m_GameOver) {
            m_GameOver = false;
            m_LevelComplete = false;
            *m_CameraTime = 0;
            m_TimeSinceLastPlatform = 0;

            // Heal the player!
            if (m_Player)
                m_Player->resetHealth();

            positionLevelAtStart();
        }

        *m_CameraTime += timeSinceLastUpdate;
        // m_TimeSinceLastPlatform += timeSinceLastUpdate; // remove recycling

        //. Platform Logic
        if (m_TimeSinceLastPlatform > m_PlatformCreationInterval) {

            // Define difficulty parameters based on level
            int minHeightChange, maxHeightChange;
            int minGap, maxGap;
            int minWidth, maxWidth;

            // 1. Get reference to the platform at the END of the line
            PlatformUpdate *refPlatform = m_Platforms[m_MoveRelativeToPlatform];
            sf::FloatRect *refRect = refPlatform->getPositionPointer();

            // 2. Get reference to the platform we are MOVING to the front
            PlatformUpdate *nextPlatform = m_Platforms[m_NextPlatformToMove];
            sf::FloatRect *nextRect = nextPlatform->getPositionPointer();

            float currentY = refRect->top;

            if (m_CurrentLevel == 1) {
                // Easy Mode
                minHeightChange = -40;
                maxHeightChange = 40;
                minGap = 20;
                maxGap = 50;
                minWidth = 150;
                maxWidth = 300;
            } else // Level 2
            {
                // Hard Mode: Higher climbs, wider jumps, smaller platforms
                minHeightChange = -100;
                maxHeightChange = 100;
                minGap = 80;
                maxGap = 140; // Harder jumps
                minWidth = 80;
                maxWidth = 200; // Smaller landing zones
            }

            // CALCULATE NEW POSITION

            // Height
            float screenHeight =
                (m_ScreenSize.y > 0) ? (float)m_ScreenSize.y : 800.f;
            float minY = screenHeight * 0.15f;
            float maxY = screenHeight - 64 - 150;

            float nextY =
                currentY + getRandomNumber(minHeightChange, maxHeightChange);

            if (nextY < minY)
                nextY = minY;
            if (nextY > maxY)
                nextY = maxY;

            nextRect->top = nextY;

            // Left (Gap)
            nextRect->left = refRect->left + refRect->width +
                             getRandomNumber(minGap, maxGap);

            // Width
            nextRect->width = getRandomNumber(minWidth, maxWidth);
            nextRect->height = 64;

            // RESURRECT THE PLATFORM
            nextPlatform->reset();

            // Update indices
            m_PlatformCreationInterval = nextRect->width / 100.f;
            m_MoveRelativeToPlatform = m_NextPlatformToMove;
            m_NextPlatformToMove++;

            if (m_NextPlatformToMove == m_NumberOfPlatforms) {
                m_NextPlatformToMove = 0;
            }
            m_TimeSinceLastPlatform = 0;
        }

        //. Game Over Logic – tied to the road, not the raw screen height

        // Check for Health Death
        if (m_Player && m_Player->isDead()) {
            m_IsPaused = true;
            m_GameOver = true;
        }

        //. Game Over Logic (Falling)
        float screenHeight =
            (m_ScreenSize.y > 0) ? (float)m_ScreenSize.y : 800.f;
        const float ROAD_TILE_HEIGHT = 64.f;
        float roadTopY = screenHeight - ROAD_TILE_HEIGHT;
        const float FALL_BUFFER = 2.f * m_PlayerPosition->height;
        float deathY = roadTopY + FALL_BUFFER;

        if (m_PlayerPosition->top > deathY) {
            m_IsPaused = true;
            m_GameOver = true;

            // UNCOMMENT THIS IF YOU WANT THE SOUND
            SoundEngine::playDeath();
        }
    }
}

// Getter for game won state
bool *LevelUpdate::getGameWonPointer()
{
    return &m_GameWon;
}

// Reset the game to initial state
void LevelUpdate::resetGame()
{
    m_CurrentLevel = 1;
    m_GameWon = false;
    m_GameOver = false;
    m_LevelComplete = false;
    *m_CameraTime = 0;
    positionLevelAtStart();
}