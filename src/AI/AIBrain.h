#pragma once
#include <vector>
#include <memory>
#include <SFML/System/Vector2.hpp>

// Forward declarations to keep compilation fast and avoid circular includes
class Footballer;
class Ball;
class GameObject;
enum class Team; // Assuming Team is defined in Entity.h or Config.h

enum class AIBrainState {
    Loose_Chasing,
    Loose_Supporting,
    Defending_Pressing,
    Defending_Covering,
    Attacking_OnBall,
    Attacking_Supporting
};

class AIBrain {
private:
    Footballer* owner;         // The player this brain controls
    Ball* targetBall;          // Reference to the match ball
    const std::vector<std::unique_ptr<GameObject>>* environment; // Reference to all players
    AIBrainState currentState;

    float passCooldownTimer = 0.f;
    AIBrainState previousState;
    Footballer* lastPassTarget = nullptr;
    int passSpamCount = 0;

    // The "Thinking" Phase
    void evaluateState();
    bool checkIfClosestTeammateToBall(float hysteresisBuffer);

    // The "Doing" Phase (State Behaviors)
    void executeLooseChasing();
    void executeLooseSupporting();
    void executeDefendingPressing();
    void executeDefendingCovering();
    void executeAttackingOnBall();
    void executeAttackingSupporting();

public:
    AIBrain(Footballer* owner, Ball* ball, const std::vector<std::unique_ptr<GameObject>>* env);
    ~AIBrain() = default;

    void update(float dt);

    AIBrainState getCurrentState() const { return currentState; }
};