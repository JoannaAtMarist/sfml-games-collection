#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Update.hpp"

// Forward declare so we can use the pointer type
class PlatformUpdate;

// -------------------------------------------------------------
// LevelUpdate
//  - Controls platform spawning, movement, timing,
//    game over detection, and player placement.
// -------------------------------------------------------------
class LevelUpdate : public Update
{
  private:
    bool m_IsPaused = true;

    // Store pointers to the logic component
    std::vector<PlatformUpdate *> m_Platforms;

    float *m_CameraTime = new float;

    sf::FloatRect *m_PlayerPosition = nullptr;

    float m_PlatformCreationInterval = 0;
    float m_TimeSinceLastPlatform = 0;

    int m_NextPlatformToMove = 0;
    int m_NumberOfPlatforms = 0;
    int m_MoveRelativeToPlatform = 0;

    bool m_GameOver = true;

    // Store the screen resolution
    sf::Vector2u m_ScreenSize;

    // Level progression variables
    int m_CurrentLevel = 1;
    const float LEVEL_LENGTH = 4000.f; // How long the level is in pixels
    bool m_LevelComplete = false;

    // Track if player won
    bool m_GameWon = false;

    // Pointer to the full player object (not just position)
    PlayerUpdate *m_Player = nullptr;

  public:
    // Take the full object, not just the position
    void addPlatform(PlatformUpdate *platform);

    void connectToCameraTime(float *cameraTime);
    bool *getIsPausedPointer();

    int getRandomNumber(int minHeight, int maxHeight);

    void update(float fps) override;
    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    void positionLevelAtStart();

    // screen resolution setter
    void setWindowSize(sf::Vector2u size);

    // Level Getter
    int getCurrentLevel() const
    {
        return m_CurrentLevel;
    }
    // Level Setter
    void setLevel(int level)
    {
        m_CurrentLevel = level;
    }

    // Getter for victory state
    bool *getGameWonPointer();

    // Function to fully reset the game from the start
    void resetGame();
};