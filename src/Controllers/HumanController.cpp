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

    // Normalize movement
    if (moveDir.x != 0.f && moveDir.y != 0.f) {
        float length = std::hypot(moveDir.x, moveDir.y);
        moveDir.x /= length;
        moveDir.y /= length;
    }

    // Apply movement
    owner->setVelocity(moveDir * owner->getStats().speed);

    // If we aren't pressing WASD, our "Aim Direction" defaults to the way we are facing
    sf::Vector2f aimDir = moveDir;
    if (aimDir.x == 0.f && aimDir.y == 0.f) {
        aimDir.x = owner->getFacingDirection();
    }


    // ==========================================
    // 2. THE 'E' KEY: Passing & Switching
    // ==========================================
    if (passKey && actionCooldown <= 0.f) {
        actionCooldown = 0.3f;
            if (owner->getPossession()) {
            // 1. Determine the exact direction the player is holding
            sf::Vector2f aimDir = moveDir;

            // If they are standing completely still, default to kicking forward!
            if (aimDir.x == 0.f && aimDir.y == 0.f) {
                // Assuming Team 0 attacks Right, Team 1 attacks Left
                aimDir.x = (owner->getFacingDirection() == 0) ? 1.f : -1.f;
            } else {
                // Normalize the aim direction
                float len = std::hypot(aimDir.x, aimDir.y);
                aimDir.x /= len;
                aimDir.y /= len;
            }

            // 2. Try to find a teammate using your dot product rule
            Footballer* bestTarget = findBestPassTarget(aimDir);

            if (bestTarget) {
                // --- NEW LOGIC: PREDICTIVE TARGETED PASS ---
                float initialDx = bestTarget->getPosition().x - owner->getPosition().x;
                float initialDy = bestTarget->getPosition().y - owner->getPosition().y;
                float initialDist = std::hypot(initialDx, initialDy);

                // Check for Lob Modifier
                bool isLob = modKey;
                float passSpeed = isLob ? 380.f : 450.f;
                float launchHeight = isLob ? 320.f : 15.f;

                // Predictive Math (Leading the receiver)
                float timeToReach = initialDist / passSpeed;
                sf::Vector2f targetVel = bestTarget->getVelocity();
                sf::Vector2f predictedPos = bestTarget->getPosition();

                if (std::hypot(targetVel.x, targetVel.y) > 10.f) {
                    predictedPos.x += (targetVel.x * timeToReach);
                    predictedPos.y += (targetVel.y * timeToReach);
                }

                // Calculate final kick trajectory
                float dx = predictedPos.x - owner->getPosition().x;
                float dy = predictedPos.y - owner->getPosition().y;
                float dist = std::hypot(dx, dy);

                // Lock onto receiver and fire!
                targetBall->setIntendedReceiver(bestTarget);

                // Note: Using your new 3-parameter kick vector (X, Y, Height)
                owner->kickBall({(dx / dist) * passSpeed, (dy / dist) * passSpeed, launchHeight});

            }
            else {
                // --- EXISTING LOGIC: THE CLEARANCE / BLIND PASS ---
                // No teammate was found in that direction. Boot it into open space!

                float clearanceSpeed = 400.f;
                float clearanceHeight = 50.f;

                // If they hold the Lob Modifier, boot it high into the air!
                if (modKey) {
                    clearanceSpeed = 250.f;  // Hangs in the air longer
                    clearanceHeight = 320.f;
                }

                targetBall->setIntendedReceiver(nullptr); // No target, it's a free ball!

                owner->kickBall({aimDir.x * clearanceSpeed, aimDir.y * clearanceSpeed, clearanceHeight});

            }

            actionCooldown = 0.3f; // Prevent input spam
        }
        // if (owner->getPossession()) {
        //     // OFFENSE: Pass the ball
        //     Footballer* bestTarget = findBestPassTarget(aimDir);
        //     if (bestTarget) {
        //
        //         float initialDx = bestTarget->getPosition().x - owner->getPosition().x;
        //         float initialDy = bestTarget->getPosition().y - owner->getPosition().y;
        //         float initialDist = std::hypot(initialDx, initialDy);
        //
        //         // 1. CHECK FOR LOB MODIFIER
        //         bool isLob = modKey;
        //
        //         // Adjust speeds: High passes hang in the air longer, so we adjust horizontal travel
        //         float passSpeed = isLob ? 380.f : 450.f;
        //         float launchHeight = isLob ? 320.f : 15.f;
        //
        //         // 2. PREDICTIVE MATH (Leading the receiver)
        //         float timeToReach = initialDist / passSpeed;
        //         sf::Vector2f targetVel = bestTarget->getVelocity();
        //         sf::Vector2f predictedPos = bestTarget->getPosition();
        //
        //         if (std::hypot(targetVel.x, targetVel.y) > 10.f) {
        //             predictedPos.x += (targetVel.x * timeToReach);
        //             predictedPos.y += (targetVel.y * timeToReach);
        //         }
        //
        //         // 3. Fire the Ball
        //         float dx = predictedPos.x - owner->getPosition().x;
        //         float dy = predictedPos.y - owner->getPosition().y;
        //         float dist = std::hypot(dx, dy);
        //
        //         targetBall->setIntendedReceiver(bestTarget);
        //
        //         // Apply the trajectory!
        //         owner->kickBall({(dx / dist) * passSpeed, (dy / dist) * passSpeed, launchHeight});
        //     }
        //
        // }
        else {
            // DEFENSE: Switch player to whoever is closest to the ball
            Footballer* closestToBall = findClosestTeammateToBall();
            if (closestToBall && teamManager) {
                teamManager->switchHumanControl(closestToBall, this->id);
            }
        }
    }


    // ==========================================
    // THE 'SPACEBAR': Shooting (With Chip Upgrade!)
    // ==========================================
    if (shootKey && actionCooldown <= 0.f) {
        actionCooldown = 0.6f; // Slight cooldown so they can't machine-gun shots

        if (owner->getPossession()) {
            // Determine aiming direction based on current WASD movement input,
            // or fallback to the direction the player is currently facing if standing still
            sf::Vector2f shotDir = moveDir;
            if (shotDir.x == 0.f && shotDir.y == 0.f) {
                shotDir.x = owner->getFacingDirection();
            } else {
                float length = std::hypot(shotDir.x, shotDir.y);
                shotDir.x /= length;
                shotDir.y /= length;
            }

            // 1. CHECK FOR CHIP MODIFIER
            bool isChip = modKey;

            // 2. ADJUST TRAJECTORY
            // A chipped shot drops forward power to scoop the ball way up into the sky!
            float shotSpeed   = isChip ? 420.f : 650.f;
            float launchHeight = isChip ? 280.f : 120.f;

            // 3. FORCE LOOSE BALL (No intended receiver on a shot!)
            if (targetBall) {
                targetBall->setIntendedReceiver(nullptr);
            }

            // 4. Unleash the Shot!
            owner->kickBall({shotDir.x * shotSpeed, shotDir.y * shotSpeed, launchHeight});

            // Loose possession immediately
            owner->setPossession(false);
        }
        else {
            // DEFENSE: TACKLING
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

// --- HELPER FUNCTIONS ---

Footballer* HumanController::findBestPassTarget(sf::Vector2f aimDir) {
    Footballer* bestTarget = nullptr;
    float bestDotProduct = -1.f;

    for (const auto& obj : *environment) {
        if (auto teammate = dynamic_cast<Footballer*>(obj.get())) {
            if (teammate != owner && teammate->getTeam() == owner->getTeam()) {

                sf::Vector2f toMate = teammate->getPosition() - owner->getPosition();
                float dist = std::hypot(toMate.x, toMate.y);

                if (dist > 10.f) {
                    toMate.x /= dist; toMate.y /= dist; // Normalize

                    // Dot product tells us how closely the aim direction matches the teammate's direction
                    float dotProduct = (aimDir.x * toMate.x) + (aimDir.y * toMate.y);

                    // Pick the teammate we are most directly aiming at
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