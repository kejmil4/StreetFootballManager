#include "Entity.h"
#include "../Core/Config.h"
#include <algorithm>

Entity::Entity(float x, float y, const EntityStats& baseStats)
    : GameObject(x, y), stats(baseStats), currentStamina(baseStats.maxStamina), velocity({0.f, 0.f}) {
}

void Entity::render(sf::RenderTarget& target) {
    target.draw(sprite);
}

// Universal Movement: Applies velocity to position using delta time (dt)
void Entity::applyMovement(float dt) {
    // 1. --- STAMINA LOGIC ---
    bool isMoving = (velocity.x != 0.f || velocity.y != 0.f);

    if (isMoving) {
        // Drain stamina (e.g., 25 points per second, takes 4 seconds to exhaust)
        currentStamina -= 25.f * dt;
        if (currentStamina < 0.f) currentStamina = 0.f;
    } else {
        // Regenerate stamina when standing still (e.g., 15 points per second)
        currentStamina += 15.f * dt;
        if (currentStamina > stats.maxStamina) currentStamina = stats.maxStamina;
    }

    // Calculate our speed penalty.
    // At 100% stamina, multiplier is 1.0. At 0% stamina, it drops to 0.5 (half speed).
    float staminaRatio = currentStamina / stats.maxStamina;
    float speedMultiplier = 0.5f + (0.5f * staminaRatio);

    // Apply the penalty to the velocity before we move!
    sf::Vector2f actualVelocity = velocity * speedMultiplier;

    // 2. --- ACTUAL MOVEMENT ---
    position += actualVelocity * dt;

    // 3. --- BOUNDARY CLAMPING ---
    float margin = 16.f;
    position.x = std::clamp(position.x, margin, Config::WINDOW_WIDTH - margin);
    position.y = std::clamp(position.y, margin, Config::WINDOW_HEIGHT - margin);

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
