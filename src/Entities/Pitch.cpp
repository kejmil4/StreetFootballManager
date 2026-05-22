#include "Pitch.h"
#include "../Core/Config.h"
#include <iostream>

Pitch::Pitch(PitchType type) : GameObject(0.f, 0.f), sprite(texture) {

    std::string texturePath;

    switch (type) {
        case PitchType::Asphalt: texturePath = "assets/pitchAsphalt.png"; break;
        case PitchType::Mud:     texturePath = "assets/pitchMud.png"; break;
        case PitchType::Grass:
        default:                 texturePath = "assets/pitch.png"; break;
    }

    if (!texture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load pitch texture: " << texturePath << "\n";
    }
    sprite.setTexture(texture, true);

    sprite.setScale({4.f, 4.f});
}

void Pitch::update(float dt) {
}

void Pitch::render(sf::RenderTarget& target) {
    target.draw(sprite);
}