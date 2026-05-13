#pragma once
#include <SFML/Graphics.hpp>
#include <memory>


class GameState; 

class Game {
private:
    sf::RenderWindow window;
    sf::Clock clock;

    std::unique_ptr<GameState> currentState;

    void processEvents();
    void update(float dt);
    void render();

public:
    Game();
    ~Game();

    void run(); 
};