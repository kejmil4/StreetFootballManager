#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "../Managers/AudioManager.h"

class GameState;

/**
 * @class Game
 * @brief The core application class.
 * Manages the main SFML render window, the application heartbeat (game loop),
 * and the top-level Game State Machine (Menu -> Match -> Career, etc.).
 */

class Game {
private:
    sf::RenderWindow window;
    sf::Clock clock;

    // Core loop components
    void processEvents();
    void update(float dt);
    void render();

    // State Machine implementation using unique pointers for automatic memory management
    std::unique_ptr<GameState> currentState;
    std::unique_ptr<GameState> nextState;

    std::unique_ptr<AudioManager> audioManager;

public:
    Game();
    ~Game();

    void run();

    AudioManager* getAudio() { return audioManager.get(); }

    /**
     * Queues a state transition to occur at the beginning of the next frame.
     * This safely prevents memory access violations that occur if a state is deleted
     * while it is still in the middle of its update loop.
     */
    void changeState(std::unique_ptr<GameState> newState);

    void closeApplication();
};