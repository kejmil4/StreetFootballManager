#include "Pitch.h"
#include "../Core/Config.h"
#include <iostream>

Pitch::Pitch() : GameObject(0.f, 0.f), sprite(texture) {

    // FIX: Catch the error so you know if the file path is wrong!
    if (!texture.loadFromFile("assets/PitchConcept.png")) {
        std::cerr << "FAILED TO LOAD: assets/PitchConcept.png\n";
    }
    sprite.setTexture(texture, true);

    // Scale your 480x270 image up by 4 to perfectly fit the 1920x1080 screen!
    sprite.setScale({4.f, 4.f});
}

void Pitch::update(float dt) {
}

void Pitch::render(sf::RenderTarget& target) {
    target.draw(sprite);
}