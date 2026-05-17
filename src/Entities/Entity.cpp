#include "Entity.h"
#include "../Core/Config.h"
#include <algorithm>

Entity::Entity(float x, float y, const EntityStats& baseStats)
: GameObject(x, y), stats(baseStats), currentStamina(baseStats.maxStamina), velocity({0.f, 0.f}), hasPossession(false),
  tackleCooldown(0.f),
  stunTimer(0.f) {
}

void Entity::render(sf::RenderTarget& target) {
    target.draw(sprite);
}

// Universal Movement: Applies velocity to position using delta time (dt)
void Entity::applyMovement(float dt) {
    // 1. --- STAMINA LOGIC ---
    bool isMoving = (velocity.x != 0.f || velocity.y != 0.f);

    if (isMoving) {
        currentStamina -= 25.f * dt;
        if (currentStamina < 0.f) currentStamina = 0.f;
    } else {
        currentStamina += 50.f * dt;
        if (currentStamina > stats.maxStamina) currentStamina = stats.maxStamina;
    }


    float staminaRatio = currentStamina / stats.maxStamina;
    float speedMultiplier = 0.5f + (0.5f * staminaRatio);

    sf::Vector2f actualVelocity = velocity * speedMultiplier;

    // 2. --- ACTUAL MOVEMENT ---
    position += actualVelocity * dt;

    // --- INVISIBLE WALL
    // X-Axis (Left and Right walls)
    if (position.x < Config::PITCH_LEFT_X) {
        position.x = Config::PITCH_LEFT_X;
    }
    else if (position.x > Config::PITCH_RIGHT_X) {
        position.x = Config::PITCH_RIGHT_X;
    }

    // Y-Axis (Top and Bottom walls)
    if (position.y < Config::PITCH_TOP_Y) {
        position.y = Config::PITCH_TOP_Y;
    }
    else if (position.y > Config::PITCH_BOTTOM_Y) {
        position.y = Config::PITCH_BOTTOM_Y;
    }

    sprite.setPosition(position);

    // Reset base velocity for the next frame
    velocity = {0.f, 0.f};
}

float Entity::getStamina() const {
    return currentStamina;
}

const EntityStats& Entity::getStats() const {
    return stats;
}

bool Entity::getPossession() const {
    return hasPossession;
}

void Entity::setPossession(bool state) {
    hasPossession = state;
}

bool Entity::canTackle() const {
    return tackleCooldown <= 0.f;
}

void Entity::resetTackleCooldown() {
    tackleCooldown = 1.5f;
}

void Entity::updateCooldowns(float dt) {
    if (tackleCooldown > 0.f) {
        tackleCooldown -= dt;
    }
    if (stunTimer > 0.f) {
        stunTimer -= dt;
    }
}

void Entity::stun(float duration) {
    stunTimer = duration;
}

bool Entity::isStunned() const {
    return stunTimer > 0.f;
}

Team Entity::getTeam() const {
    return team;
}

void Entity::setTeam(Team t) {
    team = t;
}
