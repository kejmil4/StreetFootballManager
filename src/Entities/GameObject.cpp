#include "GameObject.h"

GameObject::GameObject(float x, float y) : position({x, y}), sprite(texture) {
    // Immediately sync the visual representation to the logical starting coordinates.
    // Note: The sprite is bound to the texture in the initializer list above.
    // Child classes will safely load their specific image data into 'texture' later.
    sprite.setPosition(position);
}

sf::Vector2f GameObject::getPosition() const {
    return position;
}

sf::FloatRect GameObject::getBounds() const {
    // Uses SFML's built-in global bounds calculation, which automatically factors
    // in the sprite's current position, scale, and origin point.
    return sprite.getGlobalBounds();
}

void GameObject::resetPosition(float x, float y) {
    position = {x, y};
    sprite.setPosition(position);
}