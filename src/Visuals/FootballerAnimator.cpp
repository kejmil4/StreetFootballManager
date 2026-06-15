#include "../Visuals/FootballerAnimator.h"
#include "../Entities/Entity.h"
#include <iostream>

FootballerAnimator::FootballerAnimator(sf::Sprite& targetSprite, Team teamAlignment) {
    std::vector<sf::Color> hairPalette = {
        sf::Color(220, 180, 90),  // Classic Blonde
        sf::Color(180, 130, 60),  // Dirty Blonde / Light Brown
        sf::Color(139, 69, 19),   // Medium Brown
        sf::Color(101, 67, 33),   // Dark Brown
        sf::Color(205, 133, 63)   // Sandy / Auburn
    };

    sf::Color hair = hairPalette[rand() % hairPalette.size()];
    sf::Color jersey, shorts;

    if (teamAlignment == Team::Home) {
        jersey = sf::Color::White;
        shorts = sf::Color::Blue;
    } else {
        jersey = sf::Color::Red;
        shorts = sf::Color::Black;
    }

    // Load and Palette Swap the Texture
    this->texture = loadCustomKit("assets/footballer4frames.png", jersey, shorts, hair);

    // Apply to the Entity's Sprite
    targetSprite.setTexture(this->texture);
    targetSprite.setTextureRect({{0, 0}, {32, 32}});
    targetSprite.setOrigin({16.f, 16.f});
}

sf::Texture FootballerAnimator::loadCustomKit(const std::string& filepath, sf::Color jerseyColor, sf::Color shortsColor, sf::Color hairColor) {
    sf::Image image;
    if (!image.loadFromFile(filepath)) {
        std::cerr << "Failed to load player sprite sheet!\n";
    }

    // --- CPU Palette Swapping Algorithm ---
    // Iterate through every single pixel of the loaded image.
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
        if (teamAlignment == Team::Away) {
            targetSprite.setScale({-4.f, 4.f});
            facingDirection = -1.f;
        } else {
            targetSprite.setScale({4.f, 4.f});
            facingDirection = 1.f;
        }
    }
}