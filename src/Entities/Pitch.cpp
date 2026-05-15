#include "Pitch.h"
#include "../Core/Config.h"

Pitch::Pitch() : GameObject(0.f, 0.f) {

    sf::Image tempImage({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}, sf::Color(34, 139, 34));
    texture.loadFromImage(tempImage);

    sprite.setTextureRect({{0, 0}, {Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}});
    sprite.setPosition({0.f, 0.f});
}

void Pitch::update(float dt) {
}

void Pitch::render(sf::RenderTarget& target) {
    target.draw(sprite);
}