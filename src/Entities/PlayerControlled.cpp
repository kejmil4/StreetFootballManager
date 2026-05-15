#include "PlayerControlled.h"
#include <SFML/Window/Keyboard.hpp> // Required for keyboard polling in SFML 3.0

// PlayerControlled::PlayerControlled(float x, float y, const EntityStats& baseStats)
//     : Entity(x, y, baseStats) {
// }
PlayerControlled::PlayerControlled(float x, float y, const EntityStats& baseStats)
    : Entity(x, y, baseStats) {

    // --- TEMPORARY VISUAL FIX FOR MVP TESTING ---
    // Creates a 32x32 pixel white square in memory and loads it into our texture
    sf::Image tempImage({32, 32}, sf::Color::White);
    texture.loadFromImage(tempImage);

    sprite.setTextureRect({{0, 0}, {32, 32}});

    sprite.setOrigin({16.f, 16.f});
}

void PlayerControlled::handleInput() {
    velocity = {0.f, 0.f};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        velocity.y -= stats.speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        velocity.y += stats.speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        velocity.x -= stats.speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        velocity.x += stats.speed;
    }

    if (velocity.x != 0.f && velocity.y != 0.f) {
        velocity.x *= 0.7071f;
        velocity.y *= 0.7071f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        // TODO: Implement Shoot (if offense) or Tackle (if defense)
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
        // TODO: Implement Pass (if offense) or Switch Player (if defense)
    }
}

void PlayerControlled::update(float dt) {
    handleInput();

    applyMovement(dt);
}