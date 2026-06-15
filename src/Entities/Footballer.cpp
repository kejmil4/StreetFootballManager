#include "Footballer.h"
#include "../AI/AIBrain.h"
#include "../Core/Config.h"
#include <SFML/Window/Keyboard.hpp>
#include "../Visuals/FootballerAnimator.h"
#include "../Controllers/HumanController.h"
#include <cmath>
#include <iostream>

Footballer::Footballer(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env, bool startsAsHuman)
    : Entity(x, y, baseStats), targetBall(ball), environment(env), isHuman(startsAsHuman), targetPos(x, y), startPosition(x, y) {

    setTeam(teamAlignment);

    // Initialize all potential components. The update loop will decide which ones to actively tick.
    brain = std::make_unique<AIBrain>(this, targetBall, environment);
    humanInput = std::make_unique<HumanController>(this, environment, targetBall);
    animator = std::make_unique<FootballerAnimator>(sprite, teamAlignment);
}

Footballer::~Footballer() = default;

// --- Control State Machine ---

void Footballer::makeHuman(ControllerID id) {
    isHuman = true;
    humanID = id;
    if (humanInput) humanInput->setControllerID(id);
}

void Footballer::makeAI() {
    isHuman = false;
    // Reset velocities so an AI doesn't inherit a running sprint from a human who just switched away
    velocity = {0.f, 0.f};
}

void Footballer::kickBall(sf::Vector3f power) {
    if (targetBall && getPossession()) {
        targetBall->kick(power);
        setPossession(false);

        possessionCooldown = 0.3f;
    }
}

// --- Main Update Loop ---

void Footballer::update(float dt) {
    updateCooldowns(dt);

    if (possessionCooldown > 0.f) possessionCooldown -= dt;

    if (isStunned()) {
        velocity = {0.f, 0.f};
        return;
    }

    velocity = {0.f, 0.f};

    // --- Phase 1: Decision Making ---
    if (isHuman) {
        humanInput->update(dt);
    }
    else {
        brain->update(dt);

        float dx = targetPos.x - position.x;
        float dy = targetPos.y - position.y;
        float distance = std::hypot(dx, dy);

        if (distance > 5.f) {
            velocity.x = (dx / distance) * stats.speed;
            velocity.y = (dy / distance) * stats.speed;
        }

        // --- Phase 2: Flocking (Separation) ---
        bool ignoreFlocking = false;
        AIBrainState state = brain->getCurrentState();
        if (state == AIBrainState::Loose_Chasing || state == AIBrainState::Defending_Pressing || state == AIBrainState::Attacking_OnBall) {
            ignoreFlocking = true;
        }

        if (environment && !ignoreFlocking) {
            sf::Vector2f separationForce(0.f, 0.f);
            int neighbors = 0;
            for (const auto& obj : *environment) {
                if (auto ally = dynamic_cast<Entity*>(obj.get())) {
                    if (ally != this && ally->getTeam() == this->getTeam()) {
                        float distToAlly = std::hypot(ally->getPosition().x - position.x, ally->getPosition().y - position.y);
                        if (distToAlly > 0.1f && distToAlly < 60.f) {
                            separationForce.x += (position.x - ally->getPosition().x) / distToAlly;
                            separationForce.y += (position.y - ally->getPosition().y) / distToAlly;
                            neighbors++;
                        }
                    }
                }
            }
            if (neighbors > 0) {
                velocity.x += (separationForce.x / neighbors) * 150.f;
                velocity.y += (separationForce.y / neighbors) * 150.f;
            }
        }
    }

    // --- Phase 3. Animation ---
    animator->update(dt, velocity, sprite, getTeam(), isHuman);
    applyMovement(dt);


    // --- Ball collision and possession ---
    if (targetBall && targetBall->getCarrier() == nullptr && possessionCooldown <= 0.f) {
        if (targetBall->isGrounded()) {

            float distToBall = std::hypot(position.x - targetBall->getPosition().x,
                                          position.y - targetBall->getPosition().y);

            float currentPickupRadius = 45.f;

            Footballer* receiver = targetBall->getIntendedReceiver();
            if (receiver != nullptr) {
                if (receiver == this) {
                    currentPickupRadius = 55.f;
                }
                else if (receiver->getTeam() != this->getTeam()) {
                    currentPickupRadius = 22.f;
                }
                else {
                    currentPickupRadius = 30.f;
                }
            }

            if (distToBall < currentPickupRadius) {
                this->setPossession(true);
                targetBall->setCarrier(this);

                targetBall->setIntendedReceiver(nullptr);
            }
        }
    }

    if (getPossession()) {

        float currentFaceDir = getFacingDirection();

        //  The Dribble Math
        sf::Vector2f dribblePos = position;

        float offsetX = (currentFaceDir == 1.f) ? 25.f : 0.f;

        dribblePos.x += (currentFaceDir * offsetX);
        dribblePos.y += 55.f; // Push down to the feet of the player

        targetBall->snapToPlayer(dribblePos);
    }
}

bool Footballer::attemptTackle(Footballer* enemy) {
    if (!enemy || !enemy->getPossession()) return false;

    this->resetTackleCooldown();

    float myTacklePower = this->stats.tackling;
    float enemyAgility = enemy->getStats().speed * 0.5f;

    float successChance = 50.0f + (myTacklePower - enemyAgility);

    if (successChance > 85.0f) successChance = 85.0f;
    if (successChance < 15.0f) successChance = 15.0f;

    float roll = (rand() % 100);

    if (roll <= successChance) {
        enemy->stun(1.5f);
        enemy->setPossession(false);

        if (targetBall) {
            float dx = enemy->getPosition().x - position.x;
            float dy = enemy->getPosition().y - position.y;
            float dist = std::hypot(dx, dy);

            float dirX = (dist > 0.1f) ? (dx / dist) : getFacingDirection();
            float dirY = (dist > 0.1f) ? (dy / dist) : 0.f;

            targetBall->kick({dirX * 450.f, dirY * 450.f, 80.f});
        }

        this->possessionCooldown = 0.3f;
        return true;
    }
    else {
        this->stun(0.8f);
        return false;
    }
}


void Footballer::setInputCooldown(float time) {
    if (humanInput) {
        humanInput->setActionCooldown(time);
    }
}

void Footballer::resetToKickoff() {
    // Physically move players to their starting spot and reset all values
    position = startPosition;
    sprite.setPosition(position);

    targetPos = startPosition;

    velocity = {0.f, 0.f};
    setPossession(false);

    this->stunTimer = 0.f;
    this->tackleCooldown = 0.f;
    this->possessionCooldown = 0.f;

}

float Footballer::getFacingDirection() const {
    if (animator) return animator->getFacingDirection();
    return 1.f;
}
