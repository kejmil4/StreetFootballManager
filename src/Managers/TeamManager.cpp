#include "TeamManager.h"

void TeamManager::initializeHuman(Footballer* newHuman, ControllerID id) {
    if (!newHuman) return;
    newHuman->makeHuman(id);

    if (id == ControllerID::Player1) player1 = newHuman;
    else player2 = newHuman;
}

void TeamManager::switchHumanControl(Footballer* newHuman, ControllerID id) {
    Footballer* currentActive = (id == ControllerID::Player1) ? player1 : player2;

    if (!newHuman || newHuman == currentActive) return;

    // --- Step 1: Relinquish Control ---
    // Turn the previously controlled player back over to the AIBrain
    if (currentActive) {
        currentActive->makeAI();
    }

    // --- Step 2: Grant Control ---
    if (id == ControllerID::Player1) player1 = newHuman;
    else player2 = newHuman;

    newHuman->makeHuman(id);

    // Apply a brief input freeze to the new player. This prevents the user from
    // accidentally shooting/passing if they were mashing the button to request the switch.
    newHuman->setInputCooldown(0.5f);
}

void TeamManager::update(Ball* matchBall, const std::vector<std::unique_ptr<GameObject>>& gameObjects) {
    if (!matchBall) return;

    Footballer* currentCarrier = matchBall->getCarrier();
    if (!currentCarrier) return; // Only auto-switch if someone actually has the ball!

    // --- Auto-Switch Logic for Player 1 ---
    if (player1) {
        // If a teammate of Player 1 catches the ball, and Player 1 isn't already controlling them...
        if (currentCarrier->getTeam() == player1->getTeam() && currentCarrier != player1) {
            // ...give Player 1 control of the new carrier!
            switchHumanControl(currentCarrier, ControllerID::Player1);
        }
    }


    if (player2) {
        // If a teammate of Player 2 catches the ball, and Player 2 isn't already controlling them...
        if (currentCarrier->getTeam() == player2->getTeam() && currentCarrier != player2) {
            // ...give Player 2 control of the new carrier!
            switchHumanControl(currentCarrier, ControllerID::Player2);
        }
    }
}