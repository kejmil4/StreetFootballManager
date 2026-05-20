#pragma once
#include "Entity.h"
#include "Ball.h"
#include "../Visuals/FootballerAnimator.h"
#include "../Controllers/HumanController.h"
#include <vector>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

// Forward declaration of our new AI Brain
class AIBrain;
class HumanController;


class Footballer : public Entity {
private:
    bool isHuman;

    // --- The Puppet Masters ---

    std::unique_ptr<AIBrain> brain;
    std::unique_ptr<FootballerAnimator> animator;
    std::unique_ptr<HumanController> humanInput;
    Ball* targetBall;
    const std::vector<std::unique_ptr<GameObject>>* environment;

    sf::Vector2f startPosition;

    TeamManager* teamManager = nullptr;

    // --- Movement & AI Data ---
    sf::Vector2f targetPos;

    float facingDirection = 1.f;

    float possessionCooldown = 0.f;

public:
    Footballer(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env, bool startsAsHuman);
    ~Footballer() override; // Need explicit destructor for unique_ptr

    void update(float dt) override;

    // --- Switching Mechanics ---
    bool getIsHuman() const { return isHuman; }
    void makeHuman();
    void makeAI();

    // --- AI Puppet Controls ---
    void setTargetPos(sf::Vector2f newPos) { targetPos = newPos; }
    sf::Vector2f getTargetPos() const { return targetPos; }
    void kickBall(sf::Vector3f power);

    // --- Visual Getters ---
    float getFacingDirection() const { return facingDirection; }

    void setVelocity(sf::Vector2f v) { velocity = v; }

    void setTeamManager(TeamManager* tm) {
        this->teamManager = tm;
        if (humanInput) humanInput->setTeamManager(tm);
    }

    bool attemptTackle(Footballer* enemy);

    void setInputCooldown(float time);

    void resetToKickoff();
};