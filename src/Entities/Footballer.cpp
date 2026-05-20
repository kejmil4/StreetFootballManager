#include "Footballer.h"
#include "../AI/AIBrain.h" // Include the brain so we can use it!
#include "../Core/Config.h"
#include <SFML/Window/Keyboard.hpp>
#include "../Visuals/FootballerAnimator.h"
#include "../Controllers/HumanController.h"
#include <cmath>
#include <iostream>

// ==========================================
// FOOTBALLER LOGIC
// ==========================================

Footballer::Footballer(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env, bool startsAsHuman)
    : Entity(x, y, baseStats), targetBall(ball), environment(env), isHuman(startsAsHuman), targetPos(x, y), startPosition(x, y) {

    setTeam(teamAlignment);

    brain = std::make_unique<AIBrain>(this, targetBall, environment);
    humanInput = std::make_unique<HumanController>(this, environment, targetBall);

    animator = std::make_unique<FootballerAnimator>(sprite, teamAlignment);
}

Footballer::~Footballer() = default;

void Footballer::makeHuman() {
    isHuman = true;
}

void Footballer::makeAI() {
    isHuman = false;
}

void Footballer::kickBall(sf::Vector3f power) {
    if (targetBall && getPossession()) {
        targetBall->kick(power);
        setPossession(false);

        possessionCooldown = 0.3f;
    }
}

void Footballer::update(float dt) {
    updateCooldowns(dt);

    if (possessionCooldown > 0.f) possessionCooldown -= dt;

    if (isStunned()) {
        velocity = {0.f, 0.f};
        return;
    }

    velocity = {0.f, 0.f};

    // ==========================================
    // 1. INPUT & DECISION MAKING
    // ==========================================
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

        // ==========================================
        // 2. SEPARATION FLOCKING
        // ==========================================
        // Don't apply flocking if this player is the main actor (Chasing, Pressing, or has ball)
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

    // ==========================================
    // 3. EXECUTE VISUALS & PHYSICS
    // ==========================================
    animator->update(dt, velocity, sprite, getTeam(), isHuman);
    applyMovement(dt);

    applyMovement(dt);

    // ==========================================
    // 4. BALL COLLISION & POSSESSION
    // ==========================================

    // ADDED CHECK: Only pick it up if we aren't in a cooldown!
    if (targetBall && targetBall->getCarrier() == nullptr && possessionCooldown <= 0.f) {

        if (targetBall->isGrounded()) {
            float distToBall = std::hypot(position.x - targetBall->getPosition().x,
                                          position.y - targetBall->getPosition().y);

            if (distToBall < 50.f) {
                this->setPossession(true);
                targetBall->setCarrier(this);
            }
        }
    }

    if (getPossession()) {
        sf::Vector2f dribblePos = position;

        // VISUAL FIX: If facing left, the sprite's center might be slightly offset.
        // We push the ball a bit further to the left to compensate.
        float offsetX = (facingDirection == 1.f) ? 35.f : 45.f;

        dribblePos.x += (facingDirection * offsetX);
        dribblePos.y += 45.f; // Down to the feet

        targetBall->snapToPlayer(dribblePos);
    }
}

bool Footballer::attemptTackle(Footballer* enemy) {
    if (!enemy || !enemy->getPossession()) return false;

    // 1. Put the tackler on cooldown immediately so they can't spam spacebar
    this->resetTackleCooldown();

    // 2. THE MATH (The Dice Roll)
    // Scale the stats so they are roughly on the same level
    float myTacklePower = this->stats.tackling;
    float enemyAgility = enemy->getStats().speed * 0.5f;

    // Base 50% chance, modified by the stat difference
    float successChance = 50.0f + (myTacklePower - enemyAgility);

    // Clamp the odds so it's never a guaranteed 100% or an impossible 0%
    if (successChance > 85.0f) successChance = 85.0f;
    if (successChance < 15.0f) successChance = 15.0f;

    // Roll a random number between 0 and 99
    float roll = (rand() % 100);

    // 3. THE OUTCOME
    if (roll <= successChance) {
        // --- SUCCESS! ---
        enemy->stun(1.5f); // Stun the carrier
        enemy->setPossession(false);

        // Knock the ball loose
        if (targetBall) {
            float dx = enemy->getPosition().x - position.x;
            float dy = enemy->getPosition().y - position.y;
            float dist = std::hypot(dx, dy);

            float dirX = (dist > 0.1f) ? (dx / dist) : facingDirection;
            float dirY = (dist > 0.1f) ? (dy / dist) : 0.f;

            targetBall->kick({dirX * 450.f, dirY * 450.f, 80.f});
        }

        this->possessionCooldown = 0.3f;
        return true;
    }
    else {
        // --- FAIL! (The Whiff) ---
        // The tackler completely missed and stumbled!
        this->stun(0.8f); // Freeze the tackler for a moment so the carrier escapes
        return false;
    }
}


void Footballer::setInputCooldown(float time) {
    if (humanInput) {
        humanInput->setActionCooldown(time);
    }
}

void Footballer::resetToKickoff() {
    // 1. Physically move them to their starting spot
    position = startPosition;
    sprite.setPosition(position);

    // 2. WIPE THE AI MEMORY!
    targetPos = startPosition;

    // 3. Clear all physics and cooldowns
    velocity = {0.f, 0.f};
    setPossession(false);

    this->stunTimer = 0.f;          // Assuming stunTimer is accessible from Entity
    this->tackleCooldown = 0.f;     // Clear tackle cooldowns
    this->possessionCooldown = 0.f; // Clear pickup cooldowns

    // 4. Reset facing direction toward the enemy goal
    facingDirection = (getTeam() == Team::Home) ? 1.f : -1.f;
    sprite.setScale({facingDirection * 4.f, 4.f});
}

