#include "AIControlled.h"
#include "../Core/Config.h"
#include <cmath>

AIControlled::AIControlled(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env)
    : Entity(x, y, baseStats), currentState(AIBrainState::ChasingLoose), targetBall(ball), environment(env) {

    setTeam(teamAlignment);
    sf::Color aiColor = (teamAlignment == Team::Home) ? sf::Color::White : sf::Color::Blue;

    sf::Image tempImage({32, 32}, aiColor);
    texture.loadFromImage(tempImage);
    sprite.setTextureRect({{0, 0}, {32, 32}});
    sprite.setOrigin({16.f, 16.f});
}

void AIControlled::update(float dt) {
    updateCooldowns(dt);
    if (isStunned()) {
        velocity = {0.f, 0.f};
        return;
    }
    velocity = {0.f, 0.f};

    // --- 1. DETERMINE STATE ---
    currentState = AIBrainState::ChasingLoose; // Default state

    if (environment) {
        for (const auto& obj : *environment) {
            if (auto entity = dynamic_cast<Entity*>(obj.get())) {
                if (entity->getPossession()) {
                    // Someone has the ball! Is it our team?
                    if (entity->getTeam() == this->getTeam()) {
                        currentState = AIBrainState::OffBallOffense;
                    } else {
                        currentState = AIBrainState::Defending;
                    }
                    break;
                }
            }
        }
    }

    // --- 2. EXECUTE STATE LOGIC ---
    sf::Vector2f targetPos = position; // Default target is standing still

    switch (currentState) {
        case AIBrainState::ChasingLoose:
        case AIBrainState::Defending:
            if (targetBall) {
                targetPos = targetBall->getPosition();
            }
            break;

        case AIBrainState::OffBallOffense:
            float enemyGoalX = (getTeam() == Team::Home) ? Config::WINDOW_WIDTH : 0.f;

            targetPos.x = position.x + ((getTeam() == Team::Home) ? 200.f : -200.f);

            if (targetPos.x > Config::WINDOW_WIDTH - 200.f) targetPos.x = Config::WINDOW_WIDTH - 200.f;
            if (targetPos.x < 200.f) targetPos.x = 200.f;

            targetPos.y = position.y; // Maintain our lane
            break;
    }

    // --- 3. CALCULATE MOVEMENT TOWARD TARGET ---
    float dx = targetPos.x - position.x;
    float dy = targetPos.y - position.y;
    float distance = std::hypot(dx, dy);

    if (distance > 5.f) {
        velocity.x = (dx / distance) * stats.speed;
        velocity.y = (dy / distance) * stats.speed;
    }


    // --- 4. SEPARATION (FLOCKING) LOGIC ---
    if (environment) {
        sf::Vector2f separationForce(0.f, 0.f);
        int neighbors = 0;

        for (const auto& obj : *environment) {
            if (auto ally = dynamic_cast<Entity*>(obj.get())) {
                // Only avoid teammates, and don't avoid ourselves!
                if (ally != this && ally->getTeam() == this->getTeam()) {

                    float distToAlly = std::hypot(ally->getPosition().x - position.x, ally->getPosition().y - position.y);

                    // The "Invisible Area" (60 pixels)
                    if (distToAlly > 0.1f && distToAlly < 60.f) {
                        // Calculate a push vector away from the ally
                        separationForce.x += (position.x - ally->getPosition().x) / distToAlly;
                        separationForce.y += (position.y - ally->getPosition().y) / distToAlly;
                        neighbors++;
                    }
                }
            }
        }

        // Apply the pushback force to our velocity!
        if (neighbors > 0) {
            float separationStrength = 150.f;
            velocity.x += (separationForce.x / neighbors) * separationStrength;
            velocity.y += (separationForce.y / neighbors) * separationStrength;
        }
    }

    applyMovement(dt);
}