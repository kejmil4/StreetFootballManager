#pragma once
#include <vector>
#include <memory>
#include <SFML/System/Vector2.hpp>

// Forward declarations to keep compilation fast and avoid circular includes
class Footballer;
class Ball;
class GameObject;
enum class Team;

/**
 * Defines the tactical roles available to the AI.
 * The brain evaluates game conditions and assigns one of these states
 * dynamically every frame.
 */

enum class AIBrainState {
    Loose_Chasing,          // No team has possession; moving to secure the ball
    Loose_Supporting,       // No team has possession; positioning for potential recovery
    Defending_Pressing,     // Enemy has possession; actively trying to tackle the carrier
    Defending_Covering,     // Enemy has possession; blocking passing lanes and protecting the goal
    Attacking_OnBall,       // AI has possession; deciding to shoot, pass, or dribble
    Attacking_Supporting    // Teammate has possession; fanning out to offer passing options
};

/**
 * @class AIBrain
 * @brief Acts as a Finite State Machine (FSM) to control non-human players.
 * * Instead of complex pathfinding, this class continuously evaluates the game state
 * (ball possession, distances) to assign the bot a tactical role. This allows for
 * fluid team structures (e.g., mixing humans and AI in a 3v3 environment) as the
 * bots will automatically adapt to whatever roles are left unfulfilled.
 */

class AIBrain {
private:
    Footballer* owner;         // The player this brain controls
    Ball* targetBall;          // Reference to the match ball
    const std::vector<std::unique_ptr<GameObject>>* environment; // Reference to all entities

    AIBrainState currentState;
    AIBrainState previousState;

    // Passing cooldown and spam prevention trackers
    float passCooldownTimer = 0.f;
    Footballer* lastPassTarget = nullptr;
    int passSpamCount = 0;

    // Phase 1: Decision Making
    void evaluateState();
    bool checkIfClosestTeammateToBall(float hysteresisBuffer);

    // Phase 2: State Execution
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