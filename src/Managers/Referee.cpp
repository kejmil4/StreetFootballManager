#include "Referee.h"
#include "../Core/Config.h" // Assuming this holds your pitch dimensions
#include <iostream>

Referee::Referee(float duration) : matchDuration(duration), timeRemaining(duration){}

bool Referee::updateClock(float dt) {
    timeRemaining -= dt;
    return timeRemaining <= 0.f; // Match is over!
}

bool Referee::checkGoals(Ball* matchBall) {
    if (!matchBall) return false;

    sf::Vector2f ballPos = matchBall->getPosition();

    // Check Left Goal (Away Team scores)
    if (ballPos.x < Config::PITCH_LEFT_X) {
        awayScore++;
        std::cout << "GOAL for Away Team!\n";
        return true;
    }
    // Check Right Goal (Home Team scores)
    else if (ballPos.x > Config::PITCH_RIGHT_X) {
        homeScore++;
        std::cout << "GOAL for Home Team!\n";
        return true;
    }

    return false;
}

void Referee::resetPitch(std::vector<std::unique_ptr<GameObject>>& gameObjects, Ball* matchBall) {
    if (matchBall) {
        matchBall->resetPosition(Config::CENTER_X, Config::CENTER_Y);
        matchBall->kick({0.f, 0.f, 0.f});
        matchBall->setCarrier(nullptr);
    }

    for (auto& obj : gameObjects) {
        if (auto player = dynamic_cast<Footballer*>(obj.get())) {

            player->resetToKickoff();

        }
    }
}