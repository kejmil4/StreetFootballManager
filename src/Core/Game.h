#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "../Managers/AudioManager.h"


class GameState; 

class Game {
private:
    sf::RenderWindow window;
    sf::Clock clock;

    void processEvents();
    void update(float dt);
    void render();

    std::unique_ptr<GameState> currentState;
    std::unique_ptr<GameState> nextState;

    std::unique_ptr<AudioManager> audioManager;

public:
    Game();
    ~Game();

    void run();

    AudioManager* getAudio() { return audioManager.get(); }

    void changeState(std::unique_ptr<GameState> newState);
    void closeApplication();
};