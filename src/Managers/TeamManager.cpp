#include "TeamManager.h"

void TeamManager::initializeHuman(Footballer* startingPlayer) {
    if (startingPlayer) {
        activeHumanPlayer = startingPlayer;
        activeHumanPlayer->makeHuman();
    }
}

void TeamManager::switchHumanControl(Footballer* newHuman) {
    if (!newHuman || newHuman == activeHumanPlayer) return;

    // 1. Turn the old player back into an AI bot
    if (activeHumanPlayer) {
        activeHumanPlayer->makeAI();
    }

    // 2. Grant human control to the new player
    activeHumanPlayer = newHuman;
    activeHumanPlayer->makeHuman();

    activeHumanPlayer->setInputCooldown(0.5f);
}

void TeamManager::update(Ball* matchBall, const std::vector<std::unique_ptr<GameObject>>& environment) {
    if (!matchBall || !activeHumanPlayer) return;

    // --- Auto-Switching Logic ---
    // If our team has the ball, ensure the human is controlling the carrier!

    Team myTeam = activeHumanPlayer->getTeam();
    Footballer* currentCarrier = matchBall->getCarrier();

    if (currentCarrier && currentCarrier->getTeam() == myTeam) {
        // If a teammate caught my pass (or stole the ball), switch control to them
        if (currentCarrier != activeHumanPlayer) {
            switchHumanControl(currentCarrier);
        }
    }

    // (Optional Future Addition: If the enemy has the ball, automatically switch
    // the human to the closest defender to the ball).
}