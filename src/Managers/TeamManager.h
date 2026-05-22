#pragma once
#include "../Entities/Footballer.h"
#include "../Entities/Ball.h"
#include <vector>
#include <memory>

class TeamManager {
private:
    Footballer* player1 = nullptr;
    Footballer* player2 = nullptr;

public:
    TeamManager() = default;
    ~TeamManager() = default;

    void initializeHuman(Footballer* newHuman, ControllerID id);
    void switchHumanControl(Footballer* newHuman, ControllerID id);

    // Called every frame: watches the ball to see if an auto-switch is needed
    void update(Ball* matchBall, const std::vector<std::unique_ptr<GameObject>>& environment);

};