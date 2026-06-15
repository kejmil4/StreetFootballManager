#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class Team;

/**
 * @struct AnimationState
 * @brief Tracks the timing and framing of a 2D sprite sheet.
 */
struct AnimationState {
    int row = 0;
    int frameCount = 4;
    int currentFrame = 0;
    float frameTime = 0.15f;
    float timer = 0.f;
};

/**
 * @class FootballerAnimator
 * @brief Handles all visual rendering logic for the Footballer entities.
 * Includes a procedural animation ticker and a custom CPU-level palette swapper
 * that dynamically colors jerseys, shorts, and hair based on team alignment.
 */

class FootballerAnimator {
private:
    sf::Texture texture;
    AnimationState anim;
    float facingDirection = 1.f;

    /**
      * @brief Reads the raw pixel data of the sprite sheet and replaces specific
      * "magic" colors (Red, Blue, Magenta) with the requested dynamic colors.
      */
    sf::Texture loadCustomKit(const std::string& filepath, sf::Color jerseyColor, sf::Color shortsColor, sf::Color hairColor);

public:
    // We pass the sprite by reference so the Animator can paint directly onto it
    FootballerAnimator(sf::Sprite& targetSprite, Team teamAlignment);
    ~FootballerAnimator() = default;

    /**
      * @brief Ticks the animation timer based on physical velocity and flips the sprite horizontally.
      */
    void update(float dt, sf::Vector2f velocity, sf::Sprite& targetSprite, Team teamAlignment, bool isHuman);

    float getFacingDirection() const { return facingDirection; }
};