#include "Entity.h"

Entity::Entity(float x, float y, const EntityStats& baseStats)
    : GameObject(x, y), stats(baseStats), currentStamina(baseStats.maxStamina), velocity({0.f, 0.f}) {
}

void Entity::render(sf::RenderTarget& target) {
    target.draw(sprite);
}

// Universal Movement: Applies velocity to position using delta time (dt)
void Entity::applyMovement(float dt) {
    position += velocity * dt;
    sprite.setPosition(position);

    velocity = {0.f, 0.f};
}

float Entity::getStamina() const {
    return currentStamina;
}

const EntityStats& Entity::getStats() const {
    return stats;
}