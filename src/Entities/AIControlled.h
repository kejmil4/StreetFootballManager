#pragma once
#include "Entity.h"
#include "Ball.h"

enum class AIBrainState {
    ChasingLoose,
    OffBallOffense,
    Defending
};

class AIControlled : public Entity {
private:
    AIBrainState currentState;
    Ball* targetBall;
    const std::vector<std::unique_ptr<GameObject>>* environment;

public:
    AIControlled(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env);
    ~AIControlled() override = default;

    void update(float dt) override;
};