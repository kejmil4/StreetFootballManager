#pragma once
#include "../Entities/Footballer.h"
#include "../Entities/Ball.h"
#include <vector>
#include <memory>

/**
 * @class TeamManager
 * @brief Manages the dynamic assignment of human controllers to on-pitch entities.
 * In a fluid sports game where human players frequently pass the ball,
 * this class handles the seamless transition of swapping a bot out for a human
 * (and vice versa) without disrupting the underlying state machine.
 */
class TeamManager {
private:
    Footballer* player1 = nullptr;
    Footballer* player2 = nullptr;

public:
    TeamManager() = default;
    ~TeamManager() = default;

    /**
     * Assigns a human controller to a specific footballer at the start of a match.
     */
    void initializeHuman(Footballer* newHuman, ControllerID id);
    /**
     * Executes the control hand-off. Converts the currently controlled footballer
     * back into an AI bot, and grants human control to the newly selected footballer.
     */
    void switchHumanControl(Footballer* newHuman, ControllerID id);

    /**
     * Tick function that monitors the ball state to trigger automatic player
     * switching (e.g., when a pass is successfully received by a teammate).
     */
    void update(Ball* matchBall, const std::vector<std::unique_ptr<GameObject>>& environment);

};