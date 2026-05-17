#pragma once
#include "Entity.h"
#include "Ball.h"
#include <vector>
#include <memory>

struct AnimationState {
    int row = 0;
    int frameCount = 4;
    int currentFrame = 0;
    float frameTime = 0.15f;
    float timer = 0.f;
};
enum class AIBrainState {
    ChasingLoose,
    OnBallOffense,
    OffBallOffense,
    Defending
};

class Footballer : public Entity {
private:
    bool isHuman;

    AIBrainState currentState;
    Ball* targetBall;
    const std::vector<std::unique_ptr<GameObject>>* environment;

    AnimationState anim;
    void animate(float dt);

    float facingDirection = 1.f;

public:
    Footballer(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env, bool startsAsHuman);
    ~Footballer() override = default;

    void update(float dt) override;

    // Switching mechanics
    bool getIsHuman() const { return isHuman; }
    void makeHuman();
    void makeAI();

    float getFacingDirection() const { return facingDirection; }
};