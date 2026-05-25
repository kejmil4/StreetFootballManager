#pragma once
#include "../Entities/Ball.h"
#include "../Entities/GameObject.h"
#include "../Entities/Footballer.h"
#include <vector>
#include <memory>

class Game;

class Referee {
private:
    Game* game;

    float matchDuration;
    float timeRemaining;

    int homeScore = 0;
    int awayScore = 0;

public:
    Referee(Game* game, float duration);
    ~Referee() = default;

    // Returns true if the match is over!
    bool updateClock(float dt);

    // Checks if the ball crossed the goal line. Returns true if a goal was scored.
    bool checkGoals(Ball* matchBall);

    // Resets everyone to their starting positions after a goal
    void resetPitch(std::vector<std::unique_ptr<GameObject>>& gameObjects, Ball* matchBall);

    // Getters for the UI
    float getTimeRemaining() const { return timeRemaining; }
    int getHomeScore() const { return homeScore; }
    int getAwayScore() const { return awayScore; }
};