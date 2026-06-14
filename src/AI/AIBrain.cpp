#include "AIBrain.h"
#include "../Entities/Footballer.h"
#include "../Entities/Ball.h"
#include "../Core/Config.h"
#include <cmath>

AIBrain::AIBrain(Footballer* owner, Ball* ball, const std::vector<std::unique_ptr<GameObject>>* env)
    : owner(owner), targetBall(ball), environment(env), currentState(AIBrainState::Loose_Supporting), previousState(AIBrainState::Loose_Supporting) {
}

void AIBrain::update(float dt) {
    // Relinquish control if a human player is currently controlling this footballer
    if (owner->getIsHuman()) return;

    if (passCooldownTimer > 0.f) passCooldownTimer -= dt;

    // 1. Determine the appropriate tactical role
    evaluateState();

    // Trigger a brief pass cooldown when newly receiving the ball to prevent instant, unnatural passes
    if (currentState == AIBrainState::Attacking_OnBall && previousState != AIBrainState::Attacking_OnBall) {
        passCooldownTimer = 0.4f;
    }
    previousState = currentState;

    // 2. Execute the physical movement/actions dictated by the role
    switch (currentState) {
        case AIBrainState::Loose_Chasing:        executeLooseChasing(); break;
        case AIBrainState::Loose_Supporting:     executeLooseSupporting(); break;
        case AIBrainState::Defending_Pressing:   executeDefendingPressing(); break;
        case AIBrainState::Defending_Covering:   executeDefendingCovering(); break;
        case AIBrainState::Attacking_OnBall:     executeAttackingOnBall(); break;
        case AIBrainState::Attacking_Supporting: executeAttackingSupporting(); break;
    }
}

void AIBrain::evaluateState() {
    Team possessionTeam = targetBall->getPossessionTeam();

    // Check proximity with a 40px buffer to prevent rapid state-toggling between teammates
    bool amIClosest = checkIfClosestTeammateToBall(40.f);

    if (possessionTeam == Team::None) {
        currentState = amIClosest ? AIBrainState::Loose_Chasing : AIBrainState::Loose_Supporting;
    }
    else if (possessionTeam == owner->getTeam()) {
        currentState = (targetBall->getCarrier() == owner) ? AIBrainState::Attacking_OnBall : AIBrainState::Attacking_Supporting;
    }
    else {
        currentState = amIClosest ? AIBrainState::Defending_Pressing : AIBrainState::Defending_Covering;
    }
}

bool AIBrain::checkIfClosestTeammateToBall(float hysteresisBuffer) {
    sf::Vector2f ballPos = targetBall->getPosition();
    float dx = owner->getPosition().x - ballPos.x;
    float dy = owner->getPosition().y - ballPos.y;
    float myDist = std::hypot(dx, dy);

    // Apply hysteresis: Give the player already executing a chasing role a mathematical
    // distance discount. This prevents two equidistant players from stuttering/swapping roles constantly.
    if (currentState == AIBrainState::Loose_Chasing || currentState == AIBrainState::Defending_Pressing) {
        myDist -= hysteresisBuffer;
    }

    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {
                float t_dx = teammate->getPosition().x - ballPos.x;
                float t_dy = teammate->getPosition().y - ballPos.y;
                float mateDist = std::hypot(t_dx, t_dy);

                if (mateDist < myDist) {
                    return false; // Found a closer teammate
                }
            }
        }
    }
    return true; // Confirmed as the closest player to the ball
}

// --- STATE BEHAVIORS ---

void AIBrain::executeLooseChasing() {
    owner->setTargetPos(targetBall->getPosition());
}

void AIBrain::executeLooseSupporting() {
    sf::Vector2f ballPos = targetBall->getPosition();
    float pushDirection = (owner->getTeam() == Team::Home) ? 1.0f : -1.0f;

    // Determine vertical hierarchy among supporting players to fan out properly
    bool pushWide = false;
    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {
                if (owner->getPosition().y > teammate->getPosition().y) {
                    pushWide = true; // This AI is lower on the pitch than their teammate
                }
            }
        }
    }
    // Position diagonally behind the ball to recover rebounds or missed passes
    sf::Vector2f newTarget;
    if (pushWide) {
        newTarget = { ballPos.x + (100.f * pushDirection), ballPos.y + 150.f };
    } else {
        newTarget = { ballPos.x - (100.f * pushDirection), ballPos.y - 150.f };
    }
    owner->setTargetPos(newTarget);
}

void AIBrain::executeAttackingOnBall() {
    float goalX = (owner->getTeam() == Team::Home) ? Config::PITCH_RIGHT_X : Config::PITCH_LEFT_X;
    float goalY = Config::CENTER_Y;
    float distToGoal = std::hypot(goalX - owner->getPosition().x, goalY - owner->getPosition().y);

    // --- Phase 1: Shooting ---
    // Take a shot if within realistic scoring range (450 pixels)
    if (distToGoal < 450.f) {
        float dirX = (goalX - owner->getPosition().x) / distToGoal;
        float dirY = (goalY - owner->getPosition().y) / distToGoal;

        // Introduce minor random variance to the shot angle so bots aren't pixel-perfect
        float randOffset = ((rand() % 100) / 100.f - 0.5f) * 0.3f;

        owner->kickBall({dirX * 450.f, (dirY + randOffset) * 450.f, 150.f});
        return;
    }

    // --- Phase 2: Threat Assessment & Passing ---
    float closestEnemyDist = 9999.f;
    Footballer* bestPassTarget = nullptr;
    float bestPassScore = (owner->getTeam() == Team::Home) ? -9999.f : 9999.f;

    for (const auto& obj : *environment) {
        if (auto f = dynamic_cast<Footballer*>(obj.get())) {
            if (f != owner) {
                if (f->getTeam() != owner->getTeam()) {
                    // Track nearest enemy pressure
                    float dist = std::hypot(f->getPosition().x - owner->getPosition().x, f->getPosition().y - owner->getPosition().y);
                    if (dist < closestEnemyDist) closestEnemyDist = dist;
                } else {
                    // Find the teammate furthest down the pitch towards the enemy goal
                    if (owner->getTeam() == Team::Home) {
                        if (f->getPosition().x > bestPassScore) { bestPassScore = f->getPosition().x; bestPassTarget = f; }
                    } else {
                        if (f->getPosition().x < bestPassScore) { bestPassScore = f->getPosition().x; bestPassTarget = f; }
                    }
                }
            }
        }
    }

    // Attempt a pass if under pressure (enemy within 150px), off cooldown, and a target exists
    if (passCooldownTimer <= 0.f && closestEnemyDist < 150.f && bestPassTarget) {

        // Track passing targets to prevent two AI players from infinitely ping-ponging the ball
        if (bestPassTarget == lastPassTarget) {
            passSpamCount++;
        } else {
            lastPassTarget = bestPassTarget;
            passSpamCount = 1;
        }

        // Force a dribble and apply a heavy cooldown if ping-ponging is detected
        if (passSpamCount >= 3) {
            passCooldownTimer = 1.5f;
            passSpamCount = 0;
            owner->setTargetPos({goalX, goalY});
            return;
        }

        // Execute the pass vector towards the chosen teammate
        float dx = bestPassTarget->getPosition().x - owner->getPosition().x;
        float dy = bestPassTarget->getPosition().y - owner->getPosition().y;
        float dist = std::hypot(dx, dy);

        if (targetBall) {
            targetBall->setIntendedReceiver(bestPassTarget);
        }

        owner->kickBall({(dx / dist) * 400.f, (dy / dist) * 400.f, 15.f});
        passCooldownTimer = 0.5f; // Standard cooldown

    } else {
        // --- Phase 3: Dribbling ---
        // If passing is unavailable or not under pressure, drive directly toward the enemy goal
        owner->setTargetPos({goalX, goalY});
    }
}

void AIBrain::executeDefendingPressing() {
    Footballer* carrier = targetBall->getCarrier();
    if (!carrier) return;

    owner->setTargetPos(carrier->getPosition());

    float dist = std::hypot(carrier->getPosition().x - owner->getPosition().x,
                            carrier->getPosition().y - owner->getPosition().y);

    if (dist < 55.f && owner->canTackle()) {

        owner->attemptTackle(carrier);

    }
}


void AIBrain::executeDefendingCovering() {
    float myGoalX = (owner->getTeam() == Team::Home) ? Config::PITCH_LEFT_X : Config::PITCH_RIGHT_X;
    sf::Vector2f myGoal(myGoalX, Config::CENTER_Y);
    sf::Vector2f ballPos = targetBall->getPosition();

    // Determine vertical hierarchy to build a properly spaced defensive wall
    bool coverHigh = false;
    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {
                if (owner->getPosition().y < teammate->getPosition().y) {
                    coverHigh = true;
                }
            }
        }
    }

    // Calculate a defensive vector from our goal to the ball
    float dx = ballPos.x - myGoal.x;
    float dy = ballPos.y - myGoal.y;
    float dist = std::hypot(dx, dy);

    sf::Vector2f target;
    if (dist > 0.1f) {
        float dirX = dx / dist;
        float dirY = dy / dist;

        // Establish a baseline defensive distance 350px out from the goal.
        // If the ball breaches this perimeter, pull the defenders closer to the ball.
        float coverDist = 350.f;
        if (dist < coverDist) coverDist = dist / 1.5f;

        float baseX = myGoal.x + (dirX * coverDist);
        float baseY = myGoal.y + (dirY * coverDist);

        // Calculate perpendicular offsets to fan defenders out and block passing lanes
        float offsetX = -dirY * 150.f;
        float offsetY = dirX * 150.f;

        if (coverHigh) {
            target = {baseX + offsetX, baseY + offsetY};
        } else {
            target = {baseX - offsetX, baseY - offsetY};
        }
    } else {
        target = myGoal;
    }

    owner->setTargetPos(target);
}


void AIBrain::executeAttackingSupporting() {
    Footballer* carrier = targetBall->getCarrier();
    if (!carrier) return;
    float attackDirX = (owner->getTeam() == Team::Home) ? 1.0f : -1.0f;

    // Determine vertical hierarchy to ensure players don't stack on top of each other
    bool pushHigh = false;
    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {
                if (owner->getPosition().y < teammate->getPosition().y) {
                    pushHigh = true;
                }
            }
        }
    }

    // Push ahead of the ball carrier to offer aggressive forward passing options
    float targetX = carrier->getPosition().x + (300.f * attackDirX);
    float targetY = carrier->getPosition().y + (pushHigh ? -250.f : 250.f);

    // Clamp Y positions to keep supporting AI on the pitch
    if (targetY < Config::PITCH_TOP_Y + 50.f) targetY = Config::PITCH_TOP_Y + 50.f;
    if (targetY > Config::PITCH_BOTTOM_Y - 50.f) targetY = Config::PITCH_BOTTOM_Y - 50.f;

    owner->setTargetPos({targetX, targetY});
}