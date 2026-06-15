#pragma once
#include "GameObject.h"
#include "Entity.h"
#include <SFML/Graphics/CircleShape.hpp>

class Footballer;
class EnvironmentManager;

/**
 * @class Ball
 * @brief Represents the match ball using a "Fake 3D" (2.5D) physics system.
 * Since SFML is strictly a 2D engine, we simulate the Z-axis (height) mathematically.
 * The true physical position is tracked in 3D space, and the rendering logic visually
 * separates the ball from its shadow to create the illusion of depth and height.
 */

class Ball : public GameObject {
private:
    // --- Fake 3D Physics State ---
    // x = horizontal pitch location, y = vertical pitch location, z = height off the ground
    sf::Vector3f position3D;
    sf::Vector3f velocity3D;

    // --- Physics Tuning Parameters ---
    float gravity;       // Downward acceleration applied strictly to the Z-axis
    float bounceFactor;  // Percentage of Z-velocity retained after hitting the ground
    float friction;      // Ground friction applied to X/Y velocity when rolling

    // The shadow to sell the fake 3D effect
    sf::CircleShape shadow;

    sf::Texture ballTex;
    sf::Sprite ballSprite;

    sf::Texture shadowTex;
    sf::Sprite shadowSprite;

    // --- Gameplay State ---
    Footballer* carrier = nullptr;          // The player currently dribbling the ball
    Footballer* intendedReceiver = nullptr; // Target of a pass in progress

    EnvironmentManager* envManager = nullptr;

public:
    Ball(float x, float y, EnvironmentManager* env);
    ~Ball() override = default;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void resetPosition(float x, float y) override;

    /**
     * Applies an instantaneous 3D impulse to the ball (e.g., shooting or passing).
     */
    void kick(sf::Vector3f force);

    /**
     * Glues the ball to a player's feet during a dribble, killing its momentum.
     */
    void snapToPlayer(sf::Vector2f playerPos);


    bool isGrounded() const;
    void setCarrier(Footballer* player);
    Footballer* getCarrier() const;
    Team getPossessionTeam() const;

    void setIntendedReceiver(Footballer* player) { intendedReceiver = player; }
    Footballer* getIntendedReceiver() const { return intendedReceiver; }
};