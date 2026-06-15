#pragma once
#include "../Entities/Ball.h"
#include "../Entities/GameObject.h"
#include "../Entities/Footballer.h"
#include <vector>
#include <memory>

class Game;

/**
 * @class Referee
 * @brief The supreme arbiter of the match.
 * Responsible for enforcing the rules of the game, tracking the match timer,
 * detecting goals via coordinate thresholds, and managing the scoreline.
 * Also handles the global state reset after a goal is scored.
 */

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

    /**
     * Ticks down the match timer.
     * @return True if the time has expired (match over), false otherwise.
     */
    bool updateClock(float dt);

    /**
     * Evaluates the ball's position against the pitch boundaries to detect scoring.
     * @return True if a goal was just scored.
     */
    bool checkGoals(Ball* matchBall);

    /**
     * Resets the pitch and all dynamic entities to a kickoff state.
     */
    void resetPitch(std::vector<std::unique_ptr<GameObject>>& gameObjects, Ball* matchBall);

    // Getters for the UI
    float getTimeRemaining() const { return timeRemaining; }
    int getHomeScore() const { return homeScore; }
    int getAwayScore() const { return awayScore; }
};