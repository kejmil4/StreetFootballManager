#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Assuming Team is defined in Entity.h
enum class Team;

struct AnimationState {
    int row = 0;
    int frameCount = 4;
    int currentFrame = 0;
    float frameTime = 0.15f;
    float timer = 0.f;
};

class FootballerAnimator {
private:
    sf::Texture texture;
    AnimationState anim;
    float facingDirection = 1.f;

    // The palette swapper is now a private method of the animator!
    sf::Texture loadCustomKit(const std::string& filepath, sf::Color jerseyColor, sf::Color shortsColor, sf::Color hairColor);

public:
    // We pass the sprite by reference so the Animator can paint directly onto it
    FootballerAnimator(sf::Sprite& targetSprite, Team teamAlignment);
    ~FootballerAnimator() = default;

    // Called every frame to update frames and flipping
    void update(float dt, sf::Vector2f velocity, sf::Sprite& targetSprite, Team teamAlignment, bool isHuman);

    float getFacingDirection() const { return facingDirection; }
};