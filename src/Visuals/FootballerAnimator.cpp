#include "../Visuals/FootballerAnimator.h"
#include "../Entities/Entity.h"
#include <iostream>

FootballerAnimator::FootballerAnimator(sf::Sprite& targetSprite, Team teamAlignment) {
    // 1. Setup Colors Based on Team
    sf::Color jersey, shorts, hair;
    hair = sf::Color(220, 180, 90); // Blonde hair

    if (teamAlignment == Team::Home) {
        jersey = sf::Color::White;
        shorts = sf::Color::Blue;
    } else {
        jersey = sf::Color::Red;
        shorts = sf::Color::Black;
    }

    // 2. Load and Palette Swap the Texture
    this->texture = loadCustomKit("assets/footballer4frames.png", jersey, shorts, hair);

    // 3. Apply to the Entity's Sprite
    targetSprite.setTexture(this->texture);
    targetSprite.setTextureRect({{0, 0}, {32, 32}});
    targetSprite.setOrigin({16.f, 16.f});
}

sf::Texture FootballerAnimator::loadCustomKit(const std::string& filepath, sf::Color jerseyColor, sf::Color shortsColor, sf::Color hairColor) {
    sf::Image image;
    if (!image.loadFromFile(filepath)) {
        std::cerr << "Failed to load player sprite sheet!\n";
    }

    for (unsigned int y = 0; y < image.getSize().y; ++y) {
        for (unsigned int x = 0; x < image.getSize().x; ++x) {
            sf::Color pixelColor = image.getPixel({x, y});

            if (pixelColor == sf::Color::Red || pixelColor == sf::Color::Green) {
                image.setPixel({x, y}, jerseyColor);
            }
            else if (pixelColor == sf::Color::Blue) {
                image.setPixel({x, y}, shortsColor);
            }
            else if (pixelColor == sf::Color::Magenta) {
                image.setPixel({x, y}, hairColor);
            }
        }
    }

    sf::Texture tex;
    tex.loadFromImage(image);
    return tex;
}

void FootballerAnimator::update(float dt, sf::Vector2f velocity, sf::Sprite& targetSprite, Team teamAlignment, bool isHuman) {
    // 1. Frame progression (Are we running?)
    bool isMoving = (velocity.x != 0.f || velocity.y != 0.f);

    if (isMoving) {
        anim.timer += dt;
        if (anim.timer >= anim.frameTime) {
            anim.timer = 0.f;
            anim.currentFrame = (anim.currentFrame + 1) % anim.frameCount;
        }
    } else {
        anim.currentFrame = 0; // Lock to neutral frame when standing still
        anim.timer = 0.f;
    }

    targetSprite.setTextureRect({{anim.currentFrame * 32, 0}, {32, 32}});

    // 2. Sprite Flipping (Left/Right)
    if (velocity.x < 0.f) {
        targetSprite.setScale({-4.f, 4.f});
        facingDirection = -1.f;
    }
    else if (velocity.x > 0.f) {
        targetSprite.setScale({4.f, 4.f});
        facingDirection = 1.f;
    }
    else {
        // Stationary: face the opponent's goal by default
        if (teamAlignment == Team::Away) {
            targetSprite.setScale({-4.f, 4.f});
            facingDirection = -1.f;
        } else {
            targetSprite.setScale({4.f, 4.f});
            facingDirection = 1.f;
        }
    }

    // 3. Human vs AI highlighting
    if (isHuman) targetSprite.setColor(sf::Color::White);
    else targetSprite.setColor(sf::Color(200, 200, 200)); // Slightly darker for bots
}