#include "HumanController.h"
#include "../Entities/Footballer.h"
#include "../Entities/Ball.h"
#include "../Managers/TeamManager.h"
#include "../Core/Config.h"
#include <cmath>
#include <cstdlib> // For rand()

HumanController::HumanController(Footballer* owner, const std::vector<std::unique_ptr<GameObject>>* env, Ball* ball)
    : owner(owner), environment(env), targetBall(ball) {}

void HumanController::update(float dt) {
    if (actionCooldown > 0.f) actionCooldown -= dt;

    // 1. Calculate Movement Intent (WASD)
    sf::Vector2f moveDir(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) moveDir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) moveDir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) moveDir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveDir.x += 1.f;

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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E) && actionCooldown <= 0.f) {
        actionCooldown = 0.3f;

        if (owner->getPossession()) {
            // OFFENSE: Pass the ball
            Footballer* bestTarget = findBestPassTarget(aimDir);
            if (bestTarget) {
                // Aim directly at the target's current position
                float dx = bestTarget->getPosition().x - owner->getPosition().x;
                float dy = bestTarget->getPosition().y - owner->getPosition().y;
                float dist = std::hypot(dx, dy);
                owner->kickBall({(dx / dist) * 550.f, (dy / dist) * 550.f, 30.f});

                if (teamManager) teamManager->switchHumanControl(bestTarget);
            }
        }
        else {
            // DEFENSE: Switch player to whoever is closest to the ball
            Footballer* closestToBall = findClosestTeammateToBall();
            if (closestToBall && teamManager) {
                teamManager->switchHumanControl(closestToBall);
            }
        }
    }


    // ==========================================
    // 3. THE 'SPACEBAR': Shooting & Tackling
    // ==========================================
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && actionCooldown <= 0.f) {
        actionCooldown = 0.5f;

        if (owner->getPossession()) {
            // OFFENSE: SHOOTING (With the Inaccuracy Fix!)

            // Assuming Home attacks Right (1820) and Away attacks Left (100)
            float goalX = (owner->getTeam() == Team::Home) ? 1820.f : 100.f;
            float goalY = Config::CENTER_Y; // Aim for center of goal

            float dx = goalX - owner->getPosition().x;
            float dy = goalY - owner->getPosition().y;
            float dist = std::hypot(dx, dy);

            // Normalize base direction
            float dirX = dx / dist;
            float dirY = dy / dist;

            // THE NERF: Cone of Inaccuracy
            // The further away you are, the wider the spread.
            // If distance is 1500px, spread becomes massive.
            float inaccuracyFactor = dist / 1920.f; // Scale 0.0 to ~1.0
            float maxSpreadAngle = 0.8f; // Roughly 45 degrees in radians

            // Random angle between -spread and +spread
            float randomSpread = ((rand() % 100) / 100.f * 2.f - 1.f) * (maxSpreadAngle * inaccuracyFactor);

            // Rotate the vector by the random angle
            float finalDirX = dirX * std::cos(randomSpread) - dirY * std::sin(randomSpread);
            float finalDirY = dirX * std::sin(randomSpread) + dirY * std::cos(randomSpread);

            owner->kickBall({finalDirX * 600.f, finalDirY * 600.f, 150.f});
        }
        else {
            // DEFENSE: TACKLING
            Footballer* carrier = targetBall->getCarrier();
            if (carrier && carrier->getTeam() != owner->getTeam()) {
                float dist = std::hypot(carrier->getPosition().x - owner->getPosition().x,
                                        carrier->getPosition().y - owner->getPosition().y);

                if (dist < 60.f && owner->canTackle()) {

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