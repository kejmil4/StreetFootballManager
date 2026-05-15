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
    position += velocity * dt;

    // --- Boundary Clamping ---
    float margin = 16.f;

    position.x = std::clamp(position.x, margin, Config::WINDOW_WIDTH - margin);
    position.y = std::clamp(position.y, margin, Config::WINDOW_HEIGHT - margin);

    sprite.setPosition(position);
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