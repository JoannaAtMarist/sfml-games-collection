#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Update.hpp"
#include "InputReceiver.hpp"

// -------------------------------------------------------------
// CameraUpdate
//  - Follows the player and optionally responds to zoom input.
//  - Produces a FloatRect describing the camera's world-space view.
// -------------------------------------------------------------
class CameraUpdate : public Update
{
  private:
    sf::FloatRect m_Position;        // Camera world bounds/center
    sf::FloatRect *m_PlayerPosition; // Pointer to player bounding box

    bool m_ReceivesInput = false;
    InputReceiver *m_InputReceiver = nullptr;

  public:
    sf::FloatRect *getPositionPointer();

    void handleInput();
    InputReceiver *getInputReceiver();

    void assemble(std::shared_ptr<LevelUpdate> levelUpdate,
                  std::shared_ptr<PlayerUpdate> playerUpdate) override;

    void update(float fps) override;
};