#include "Pitch.h"
#include "../Core/Config.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>

Pitch::Pitch(PitchType type, int logoId) : GameObject(0.f, 0.f), sprite(texture), hasLogo(false), logoSprite(logoTexture) {

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

    if (logoId >= 0) {
        std::string logoPath;
        switch (logoId) {
            case 0: logoPath = "assets/logos/logo1.png"; break;
            case 1: logoPath = "assets/logos/logo2.png"; break;
            case 2: logoPath = "assets/logos/logo3.png"; break;
            case 3: logoPath = "assets/logos/logo4.png"; break;
            case 4: logoPath = "assets/logos/logo5.png"; break;
            case 5: logoPath = "assets/logos/logo6.png"; break;
            case 6: logoPath = "assets/logos/logo7.png"; break;
            case 7: logoPath = "assets/logos/logo8.png"; break;
            case 9: logoPath = "assets/logos/logo9.png"; break;
            case 10: logoPath = "assets/logos/logo10.png"; break;
            case 11: logoPath = "assets/logos/logo11.png"; break;
            case 12: logoPath = "assets/logos/logo12.png"; break;
            case 13: logoPath = "assets/logos/logo13.png"; break;
            case 14: logoPath = "assets/logos/logo14.png"; break;
            case 15: logoPath = "assets/logos/logo15.png"; break;
            case 16: logoPath = "assets/logos/logo16.png"; break;
            case 17: logoPath = "assets/logos/logo17.png"; break;
                default: logoPath = "assets/logos/logo12.png"; break;
        }

        if (logoTexture.loadFromFile(logoPath)) {
            logoSprite.setTexture(logoTexture, true);

            sf::FloatRect bounds = logoSprite.getLocalBounds();
            logoSprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});

            // Put it dead center of the screen
            logoSprite.setPosition({Config::CENTER_X, Config::CENTER_Y - 415.f});

            // Scale it up so it covers the center circle
            logoSprite.setScale({0.32f, 0.32f});


            hasLogo = true;
        }
    }
}

void Pitch::update(float dt) {
}

void Pitch::render(sf::RenderTarget& target) {
    target.draw(sprite);

    if (hasLogo) {
       target.draw(logoSprite);
    }
}