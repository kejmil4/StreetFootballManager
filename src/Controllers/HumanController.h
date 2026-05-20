#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <memory>
#include "../Entities/GameObject.h"

class Footballer;
class TeamManager;
class Ball;

class HumanController {
private:
    Footballer* owner;
    const std::vector<std::unique_ptr<GameObject>>* environment;
    Ball* targetBall;
    TeamManager* teamManager = nullptr; // Used for switching players

    float actionCooldown = 0.f;

    // Helper Functions for the new mechanics
    Footballer* findBestPassTarget(sf::Vector2f aimDir);
    Footballer* findClosestTeammateToBall();

public:
    HumanController(Footballer* owner, const std::vector<std::unique_ptr<GameObject>>* env, Ball* ball);
    ~HumanController() = default;

    void setTeamManager(TeamManager* tm) { teamManager = tm; }
    void update(float dt);

    void setActionCooldown(float time) { actionCooldown = time; }
};