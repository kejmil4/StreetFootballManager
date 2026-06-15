#pragma once
#include <SFML/Graphics.hpp>

// Forward declaration to prevent circular inclusion
class Game;

/**
 * @class GameState
 * @brief Abstract base class for the Game State Machine.
 * * This class dictates the structural contract for any major "screen" or "mode"
 * in the application (e.g., MenuState, MatchState, CareerState). By inheriting
 * from this, the core Game engine can polymorphically update and render different
 * states without needing to know their specific internal logic.
 */
class GameState {
protected:
    // Pointer back to the central Game engine manager.
    // Protected so child states can use it to trigger global actions, like playing
    // audio via the AudioManager, or queuing a transition to a completely new state
    // (e.g., a MenuState telling the Game to swap to a MatchState).
    Game* game;

public:
    GameState(Game* gameEngine) : game(gameEngine) {}
    virtual ~GameState() = default;

    /**
     * @brief Processes localized hardware input.
     * Separating this from the main update loop ensures that button presses/releases
     * are handled cleanly as discrete events, rather than continuous holds.
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Advances the state's internal logic and physics.
     * @param dt Delta time (seconds) since the last frame, ensuring all movement
     * is frame-rate independent.
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Draws the state's visual elements to the screen.
     * @param target The SFML window or render surface to draw onto.
     */
    virtual void render(sf::RenderTarget& target) = 0;
};