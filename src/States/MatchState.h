#pragma once
#include "GameState.h"
#include "../Entities/GameObject.h"
#include "../Entities/Entity.h"
#include "../Entities/Ball.h"
#include <vector>
#include <memory>

class MatchState : public GameState {
private:
    // THE SINGLE CONTAINER RULE
    // This holds Players, AI, the Ball, and the Pitch all in one place!
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    Ball* matchBall = nullptr;
    Entity* ballCarrier = nullptr;

public:
    MatchState();
    ~MatchState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};