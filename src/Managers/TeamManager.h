#pragma once
#include "../Entities/Footballer.h"
#include "../Entities/Ball.h"
#include <vector>
#include <memory>

class TeamManager {
private:
    Footballer* activeHumanPlayer = nullptr;

public:
    TeamManager() = default;
    ~TeamManager() = default;

    // Call this once during initialization to set the first human player
    void initializeHuman(Footballer* startingPlayer);

    // Forces control to swap to a specific player
    void switchHumanControl(Footballer* newHuman);

    // Called every frame: watches the ball to see if an auto-switch is needed
    void update(Ball* matchBall, const std::vector<std::unique_ptr<GameObject>>& environment);

    Footballer* getActiveHuman() const { return activeHumanPlayer; }
};