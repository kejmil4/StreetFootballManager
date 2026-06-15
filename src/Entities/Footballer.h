#pragma once
#include "Entity.h"
#include "Ball.h"
#include "../Visuals/FootballerAnimator.h"
#include "../Controllers/HumanController.h"
#include <vector>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

class AIBrain;
class HumanController;

/**
 * @class Footballer
 * @brief The core actor in the game world.
 * Acts as a centralized container that binds together the physical body (Entity base class),
 * the visual representation (FootballerAnimator), and the decision-making brain
 * (either AIBrain or HumanController).
 */
class Footballer : public Entity {
private:
    bool isHuman;

    // --- Components ---
    std::unique_ptr<AIBrain> brain;
    std::unique_ptr<FootballerAnimator> animator;
    std::unique_ptr<HumanController> humanInput;

    Ball* targetBall;
    const std::vector<std::unique_ptr<GameObject>>* environment;

    sf::Vector2f startPosition;

    TeamManager* teamManager = nullptr;

    sf::Vector2f targetPos;

    float possessionCooldown = 0.f;

    ControllerID humanID = ControllerID::Player1;

public:
    Footballer(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env, bool startsAsHuman);
    ~Footballer() override; // Explicit destructor required for unique_ptr forward declarations

    void update(float dt) override;

    // --- Switching Mechanics ---
    bool getIsHuman() const { return isHuman; }
    void makeHuman(ControllerID id = ControllerID::Player1);
    void makeAI();
    ControllerID getHumanID() const { return humanID; }

    // --- AI Puppet Controls ---
    void setTargetPos(sf::Vector2f newPos) { targetPos = newPos; }
    sf::Vector2f getTargetPos() const { return targetPos; }
    void kickBall(sf::Vector3f power);

    // --- Visual Getters ---
    float getFacingDirection() const;

    void setVelocity(sf::Vector2f v) { velocity = v; }

    void setTeamManager(TeamManager* tm) {
        this->teamManager = tm;
        if (humanInput) humanInput->setTeamManager(tm);
    }

    bool attemptTackle(Footballer* enemy);

    void setInputCooldown(float time);

    void resetToKickoff();

    sf::Vector2f getVelocity() const { return velocity; }
};