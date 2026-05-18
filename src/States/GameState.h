#pragma once
#include <SFML/Graphics.hpp>

class Game;

class GameState {
protected:
    Game* game;

public:
    GameState(Game* gameEngine) : game(gameEngine) {}
    virtual ~GameState() = default;

    virtual void handleInput(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;
};