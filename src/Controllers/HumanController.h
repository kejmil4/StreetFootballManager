#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <memory>
#include "../Entities/GameObject.h"

class Footballer;
class TeamManager;
class Ball;

enum class ControllerID { Player1, Player2 };

class HumanController {
private:
    Footballer* owner;
    const std::vector<std::unique_ptr<GameObject>>* environment;
    Ball* targetBall;
    TeamManager* teamManager = nullptr;

    ControllerID id;

    float actionCooldown = 0.f;

    Footballer* findBestPassTarget(sf::Vector2f aimDir);
    Footballer* findClosestTeammateToBall();

public:
    HumanController(Footballer* owner, const std::vector<std::unique_ptr<GameObject>>* env, Ball* ball, ControllerID id = ControllerID::Player1);
    ~HumanController() = default;

    void setTeamManager(TeamManager* tm) { teamManager = tm; }
    void update(float dt);

    void setActionCooldown(float time) { actionCooldown = time; }

    void setControllerID(ControllerID newID) { id = newID; }
    ControllerID getControllerID() const { return id; }
};