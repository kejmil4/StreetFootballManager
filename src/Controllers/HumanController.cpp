#include "HumanController.h"
#include "../Entities/Footballer.h"
#include "../Entities/Ball.h"
#include "../Managers/TeamManager.h"
#include "../Core/Config.h"
#include <cmath>

HumanController::HumanController(Footballer* owner, const std::vector<std::unique_ptr<GameObject>>* env, Ball* ball, ControllerID id)
    : owner(owner), environment(env), targetBall(ball), id(id) {}

void HumanController::update(float dt) {
    if (actionCooldown > 0.f) actionCooldown -= dt;

    sf::Vector2f moveDir(0.f, 0.f);
    bool passKey, shootKey, modKey;

    // --- 1. Hardware Input Polling ---
    // Read key states based on which local player this controller belongs to
    if (id == ControllerID::Player1) {
        if (sf::Keyboard::isKeyPressed(Config::p1Binds.up)) moveDir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(Config::p1Binds.down)) moveDir.y += 1.f;
        if (sf::Keyboard::isKeyPressed(Config::p1Binds.left)) moveDir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(Config::p1Binds.right)) moveDir.x += 1.f;

        passKey  = sf::Keyboard::isKeyPressed(Config::p1Binds.passSwitch);
        shootKey = sf::Keyboard::isKeyPressed(Config::p1Binds.shootTackle);
        modKey   = sf::Keyboard::isKeyPressed(Config::p1Binds.lobModifier);
    } else {
        if (sf::Keyboard::isKeyPressed(Config::p2Binds.up)) moveDir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(Config::p2Binds.down)) moveDir.y += 1.f;
        if (sf::Keyboard::isKeyPressed(Config::p2Binds.left)) moveDir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(Config::p2Binds.right)) moveDir.x += 1.f;

        passKey  = sf::Keyboard::isKeyPressed(Config::p2Binds.passSwitch);
        shootKey = sf::Keyboard::isKeyPressed(Config::p2Binds.shootTackle);
        modKey   = sf::Keyboard::isKeyPressed(Config::p2Binds.lobModifier);
    }

    // --- 2. Movement Application ---
    // Normalize diagonal movement to prevent players from running faster
    // when pressing two keys simultaneously (the "strafing speed boost" bug).
    if (moveDir.x != 0.f && moveDir.y != 0.f) {
        float length = std::hypot(moveDir.x, moveDir.y);
        moveDir.x /= length;
        moveDir.y /= length;
    }

    // Apply movement
    owner->setVelocity(moveDir * owner->getStats().speed);

    // Track intent: If the player isn't moving, their "aim" defaults to the direction their sprite is facing
    sf::Vector2f aimDir = moveDir;
    if (aimDir.x == 0.f && aimDir.y == 0.f) {
        aimDir.x = owner->getFacingDirection();
    }


    // --- 3. Contextual Action: Pass (Offense) / Switch (Defense) ---
    if (passKey && actionCooldown <= 0.f) {
        actionCooldown = 0.3f;// Prevent input spam

            if (owner->getPossession()) {

                // Re-evaluate aim direction for the pass specifically
                sf::Vector2f aimDir = moveDir;
                if (aimDir.x == 0.f && aimDir.y == 0.f) {
                    // Default to attacking direction if standing completely still
                    aimDir.x = (owner->getFacingDirection() == 0) ? 1.f : -1.f;
                } else {
                    // Normalize the aim direction
                    float len = std::hypot(aimDir.x, aimDir.y);
                    aimDir.x /= len;
                    aimDir.y /= len;
                }

                // Attempt to find a teammate in the general direction the player is aiming
                Footballer* bestTarget = findBestPassTarget(aimDir);

                if (bestTarget) {
                    // TARGETED PASS (Leading the receiver)
                    float initialDx = bestTarget->getPosition().x - owner->getPosition().x;
                    float initialDy = bestTarget->getPosition().y - owner->getPosition().y;
                    float initialDist = std::hypot(initialDx, initialDy);

                    bool isLob = modKey;
                    float passSpeed = isLob ? 380.f : 450.f;
                    float launchHeight = isLob ? 320.f : 15.f;

                    // Predictive Physics: Calculate how long the ball will take to reach the target,
                    // and aim for where the target *will* be, rather than where they are right now.
                    float timeToReach = initialDist / passSpeed;
                    sf::Vector2f targetVel = bestTarget->getVelocity();
                    sf::Vector2f predictedPos = bestTarget->getPosition();

                    if (std::hypot(targetVel.x, targetVel.y) > 10.f) {
                        predictedPos.x += (targetVel.x * timeToReach);
                        predictedPos.y += (targetVel.y * timeToReach);
                    }

                    // Calculate final kick vector towards the predicted position
                    float dx = predictedPos.x - owner->getPosition().x;
                    float dy = predictedPos.y - owner->getPosition().y;
                    float dist = std::hypot(dx, dy);

                    targetBall->setIntendedReceiver(bestTarget);

                    owner->kickBall({(dx / dist) * passSpeed, (dy / dist) * passSpeed, launchHeight});

                }
                else {
                    // BLIND CLEARANCE
                    // No teammate was found in the aim direction. Boot the ball into open space.
                    float clearanceSpeed = 400.f;
                    float clearanceHeight = 50.f;

                    // Modifier hangs the ball high in the air to allow the team to push up
                    if (modKey) {
                        clearanceSpeed = 250.f;
                        clearanceHeight = 320.f;
                    }

                    targetBall->setIntendedReceiver(nullptr); // Release ball to open play

                    owner->kickBall({aimDir.x * clearanceSpeed, aimDir.y * clearanceSpeed, clearanceHeight});

                }

                // actionCooldown = 0.3f; // Prevent input spam
        }
        else {
            // DEFENSE: Surrender control of current player and jump to the one nearest the ball
            Footballer* closestToBall = findClosestTeammateToBall();
            if (closestToBall && teamManager) {
                teamManager->switchHumanControl(closestToBall, this->id);
            }
        }
    }


    // --- 4. Contextual Action: Shoot (Offense) / Tackle (Defense) ---
    if (shootKey && actionCooldown <= 0.f) {
        actionCooldown = 0.6f; // Longer cooldown for heavy actions

        if (owner->getPossession()) {
            sf::Vector2f shotDir = moveDir;
            if (shotDir.x == 0.f && shotDir.y == 0.f) {
                shotDir.x = owner->getFacingDirection();
            } else {
                float length = std::hypot(shotDir.x, shotDir.y);
                shotDir.x /= length;
                shotDir.y /= length;
            }

            bool isChip = modKey;

            // A chipped shot sacrifices forward velocity to scoop the ball high over the players
            float shotSpeed   = isChip ? 420.f : 650.f;
            float launchHeight = isChip ? 280.f : 120.f;

            // It's a loose ball
            if (targetBall) {
                targetBall->setIntendedReceiver(nullptr);
            }

            owner->kickBall({shotDir.x * shotSpeed, shotDir.y * shotSpeed, launchHeight});
            owner->setPossession(false);
        }
        else {
            // DEFENSE: Attempt to steal the ball from the enemy carrier
            Footballer* carrier = targetBall->getCarrier();
            if (carrier && carrier->getTeam() != owner->getTeam()) {
                float dist = std::hypot(carrier->getPosition().x - owner->getPosition().x,
                                        carrier->getPosition().y - owner->getPosition().y);

                if (dist < 80.f && owner->canTackle()) {
                    owner->attemptTackle(carrier);
                }
            }
        }
    }
}

// --- AIM ASSIST HELPERS ---

/**
 * Evaluates all teammates and selects the best passing target based on the player's
 * directional input. Uses vector math (Dot Product) to determine alignment.
 */
Footballer* HumanController::findBestPassTarget(sf::Vector2f aimDir) {
    Footballer* bestTarget = nullptr;
    float bestDotProduct = -1.f;

    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {

                sf::Vector2f toMate = teammate->getPosition() - owner->getPosition();
                float dist = std::hypot(toMate.x, toMate.y);

                if (dist > 10.f) {
                    toMate.x /= dist; toMate.y /= dist;// Normalize the vector pointing to the teammate

                    // The Dot Product returns a value between -1 and 1 indicating how parallel two vectors are.
                    // 1.0 = Perfect alignment. We use this to find the teammate the player is most directly aiming at.
                    float dotProduct = (aimDir.x * toMate.x) + (aimDir.y * toMate.y);

                    // Threshold of 0.6 (~53 degrees) ensures we don't accidentally pass backward or blindly sideways
                    if (dotProduct > bestDotProduct && dotProduct > 0.6f) {
                        bestDotProduct = dotProduct;
                        bestTarget = teammate;
                    }
                }
            }
        }
    }
    return bestTarget;
}

Footballer* HumanController::findClosestTeammateToBall() {
    Footballer* closestTeammate = nullptr;
    float closestDist = 99999.f;

    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {
                float dist = std::hypot(teammate->getPosition().x - targetBall->getPosition().x,
                                        teammate->getPosition().y - targetBall->getPosition().y);

                if (dist < closestDist) {
                    closestDist = dist;
                    closestTeammate = teammate;
                }
            }
        }
    }
    return closestTeammate;
}