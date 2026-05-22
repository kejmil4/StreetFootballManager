#include "AIBrain.h"
#include "../Entities/Footballer.h"
#include "../Entities/Ball.h"
#include "../Core/Config.h"
#include <cmath>

AIBrain::AIBrain(Footballer* owner, Ball* ball, const std::vector<std::unique_ptr<GameObject>>* env)
    : owner(owner), targetBall(ball), environment(env), currentState(AIBrainState::Loose_Supporting), previousState(AIBrainState::Loose_Supporting) {
}

void AIBrain::update(float dt) {
    if (owner->getIsHuman()) return;

    if (passCooldownTimer > 0.f) passCooldownTimer -= dt;

    evaluateState();

    if (currentState == AIBrainState::Attacking_OnBall && previousState != AIBrainState::Attacking_OnBall) {
        passCooldownTimer = 0.4f;
    }
    previousState = currentState;

    // 2. Execute the specific behavior
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
    bool amIClosest = checkIfClosestTeammateToBall(40.f); // 40px anti-flicker buffer

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

    // Give current chasers a mathematical discount to prevent role-flickering
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
                    return false; // A teammate is closer
                }
            }
        }
    }
    return true; // I am the closest!
}

// --- STATE BEHAVIORS ---

void AIBrain::executeLooseChasing() {
    owner->setTargetPos(targetBall->getPosition());
}

void AIBrain::executeLooseSupporting() {
    sf::Vector2f ballPos = targetBall->getPosition();
    float pushDirection = (owner->getTeam() == Team::Home) ? 1.0f : -1.0f;

    // Find if I am the "top" or "bottom" supporting player
    bool pushWide = false;
    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {
                if (owner->getPosition().y > teammate->getPosition().y) {
                    pushWide = true;
                }
            }
        }
    }

    sf::Vector2f newTarget;
    if (pushWide) {
        newTarget = { ballPos.x + (100.f * pushDirection), ballPos.y + 150.f };
    } else {
        newTarget = { ballPos.x - (100.f * pushDirection), ballPos.y - 150.f };
    }
    owner->setTargetPos(newTarget);
}

void AIBrain::executeAttackingOnBall() {
    // Determine where the enemy goal is
    float goalX = (owner->getTeam() == Team::Home) ? Config::PITCH_RIGHT_X : Config::PITCH_LEFT_X;
    float goalY = Config::CENTER_Y;

    float distToGoal = std::hypot(goalX - owner->getPosition().x, goalY - owner->getPosition().y);

    // 1. THE NEW SHOOTING LOGIC!
    if (distToGoal < 450.f) { // If within 450 pixels of the goal, take a shot!
        float dirX = (goalX - owner->getPosition().x) / distToGoal;
        float dirY = (goalY - owner->getPosition().y) / distToGoal;

        // Add a tiny bit of random inaccuracy so they don't always snipe the exact center
        float randOffset = ((rand() % 100) / 100.f - 0.5f) * 0.3f;

        owner->kickBall({dirX * 450.f, (dirY + randOffset) * 450.f, 150.f});
        return; // They shot the ball, so stop thinking!
    }

    // 2. THE PANIC/PASSING LOGIC (Keep your existing passing logic here)
    float closestEnemyDist = 9999.f;
    Footballer* bestPassTarget = nullptr;
    float bestPassScore = (owner->getTeam() == Team::Home) ? -9999.f : 9999.f;

    for (const auto& obj : *environment) {
        if (auto f = dynamic_cast<Footballer*>(obj.get())) {
            if (f != owner) {
                if (f->getTeam() != owner->getTeam()) {
                    float dist = std::hypot(f->getPosition().x - owner->getPosition().x, f->getPosition().y - owner->getPosition().y);
                    if (dist < closestEnemyDist) closestEnemyDist = dist;
                } else {
                    if (owner->getTeam() == Team::Home) {
                        if (f->getPosition().x > bestPassScore) { bestPassScore = f->getPosition().x; bestPassTarget = f; }
                    } else {
                        if (f->getPosition().x < bestPassScore) { bestPassScore = f->getPosition().x; bestPassTarget = f; }
                    }
                }
            }
        }
    }

    // THE NEW PASS EXECUTION & SPAM PREVENTION
    if (passCooldownTimer <= 0.f && closestEnemyDist < 150.f && bestPassTarget) {

        // 1. Check if we are spamming the same teammate
        if (bestPassTarget == lastPassTarget) {
            passSpamCount++;
        } else {
            lastPassTarget = bestPassTarget;
            passSpamCount = 1;
        }

        // 2. If we passed to the same guy 3 times, FORCE A DRIBBLE!
        if (passSpamCount >= 3) {
            passCooldownTimer = 1.5f; // Lock out passing for 1.5 seconds!
            passSpamCount = 0;        // Reset the count

            owner->setTargetPos({goalX, goalY}); // Force them to drive forward
            return;
        }

        // 3. Execute the Pass
        float dx = bestPassTarget->getPosition().x - owner->getPosition().x;
        float dy = bestPassTarget->getPosition().y - owner->getPosition().y;
        float dist = std::hypot(dx, dy);

        if (targetBall) {
            targetBall->setIntendedReceiver(bestPassTarget);
        }

        owner->kickBall({(dx / dist) * 400.f, (dy / dist) * 400.f, 15.f});

        // Put passing on cooldown so they can't machine-gun it
        passCooldownTimer = 0.5f;

    } else {
        // THE DRIBBLE LOGIC
        // If passing is on cooldown, or no enemies are near, drive toward the goal!
        owner->setTargetPos({goalX, goalY});
    }
}


// --- STATE BEHAVIORS ---

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
    // Goal: Form a defensive wall between the ball and our goal.
    float myGoalX = (owner->getTeam() == Team::Home) ? Config::PITCH_LEFT_X : Config::PITCH_RIGHT_X;
    sf::Vector2f myGoal(myGoalX, Config::CENTER_Y);
    sf::Vector2f ballPos = targetBall->getPosition();

    // 1. Figure out if I am the "Top" or "Bottom" covering defender
    bool coverHigh = false;
    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {
                if (owner->getPosition().y < teammate->getPosition().y) {
                    coverHigh = true; // Smaller Y means higher on the screen
                }
            }
        }
    }

    // 2. Draw a line from our goal to the ball
    float dx = ballPos.x - myGoal.x;
    float dy = ballPos.y - myGoal.y;
    float dist = std::hypot(dx, dy);

    sf::Vector2f target;
    if (dist > 0.1f) {
        float dirX = dx / dist;
        float dirY = dy / dist;

        // Base position: 350 pixels out from our goal, looking at the ball.
        float coverDist = 350.f;
        // If the ball is closer than 350px, don't run past it! Stand halfway.
        if (dist < coverDist) coverDist = dist / 1.5f;

        float baseX = myGoal.x + (dirX * coverDist);
        float baseY = myGoal.y + (dirY * coverDist);

        // 3. Fan out perpendicular to the ball to block passing lanes
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

    // Goal: Run forward and fan out wide to give the ball carrier passing options
    float attackDirX = (owner->getTeam() == Team::Home) ? 1.0f : -1.0f;

    // 1. Figure out if I am the "Top" or "Bottom" attacking support
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

    // 2. Push 300 pixels AHEAD of the ball carrier, and 250 pixels WIDE (up or down)
    float targetX = carrier->getPosition().x + (300.f * attackDirX);
    float targetY = carrier->getPosition().y + (pushHigh ? -250.f : 250.f);

    // 3. Prevent them from running off the top/bottom edges of the pitch
    if (targetY < Config::PITCH_TOP_Y + 50.f) targetY = Config::PITCH_TOP_Y + 50.f;
    if (targetY > Config::PITCH_BOTTOM_Y - 50.f) targetY = Config::PITCH_BOTTOM_Y - 50.f;

    owner->setTargetPos({targetX, targetY});
}