#include "GameObject.h"

GameObject::GameObject(float x, float y) : position({x, y}), sprite(texture) {
    sprite.setPosition(position);
}

sf::Vector2f GameObject::getPosition() const {
    return position;
}

sf::FloatRect GameObject::getBounds() const {
    return sprite.getGlobalBounds();
}