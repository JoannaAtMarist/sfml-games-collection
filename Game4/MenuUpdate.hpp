#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Update.hpp"
#include "InputReceiver.hpp"

class LevelUpdate; // Forward declaration

// -------------------------------------------------------------
// MenuUpdate
//  - Handles toggling visibility of menu, pausing/unpausing,
//    and following the player when visible.
// -------------------------------------------------------------
class MenuUpdate : public Update
{
  private:
    sf::FloatRect m_Position;

    InputReceiver m_InputReceiver;
    sf::FloatRect *m_PlayerPosition = nullptr;

    bool m_IsVisible = false;
    bool *m_IsPaused = nullptr;
    bool m_GameOver = false;

    sf::RenderWindow *m_Window = nullptr;

    // Pointers to Level logic
    LevelUpdate *m_LevelUpdate = nullptr;
    bool *m_GameWon = nullptr;

    // Distinguish between starting the game and dying
    bool m_FirstLoad = true;

  public:
    MenuUpdate(sf::RenderWindow *window);

    void handleInput();
    sf::FloatRect *getPositionPointer();
    bool *getGameOverPointer();
    InputReceiver *getInputReceiver();

    bool *getIsPausedPointer();

    void update(float fps) override;
    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    // Getter for Graphics
    bool *getGameWonPointer();

    // Getter
    bool *getFirstLoadPointer()
    {
        return &m_FirstLoad;
    }
};
