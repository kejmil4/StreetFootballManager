#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <memory>
#include "../Entities/GameObject.h"

// Forward declarations
class Footballer;
class TeamManager;
class Ball;

/**
 * @enum ControllerID
 * @brief Differentiates between local players to map the correct hardware inputs.
 */
enum class ControllerID { Player1, Player2 };

/**
 * @class HumanController
 * @brief Translates raw player inputs into in-game actions for a Footballer.
 * Handles movement mapping, context-sensitive actions (passing vs. switching,
 * shooting vs. tackling), and aiming assistance (dot-product targeting).
 * Supports local multiplayer by querying different keybind configurations based on ID.
 */
class HumanController {
private:
    Footballer* owner;
    const std::vector<std::unique_ptr<GameObject>>* environment;
    Ball* targetBall;
    TeamManager* teamManager = nullptr;

    ControllerID id;

    // Prevents the player from spamming inputs (like machine-gun shooting)
    float actionCooldown = 0.f;

    // Aim Assist & Targeting
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