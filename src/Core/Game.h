#pragma once
#include <SFML/Graphics.hpp>
#include <memory>


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

public:
    Game();
    ~Game();

    void run();

    void changeState(std::unique_ptr<GameState> newState);
    void closeApplication();
};