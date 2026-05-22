#pragma once
#include "GameObject.h"
#include "Entity.h"
#include <SFML/Graphics/CircleShape.hpp>

class Footballer;
class EnvironmentManager;

class Ball : public GameObject {
private:
    // A 3D vector to hold our true coordinates in the game world
    sf::Vector3f position3D;
    sf::Vector3f velocity3D;

    // Physics parameters
    float gravity;
    float bounceFactor;
    float friction;

    // The shadow to sell the fake 3D effect
    sf::CircleShape shadow;

    sf::Texture ballTex;
    sf::Sprite ballSprite;

    sf::Texture shadowTex;
    sf::Sprite shadowSprite;

    Footballer* carrier = nullptr;

    Footballer* intendedReceiver = nullptr;

    EnvironmentManager* envManager = nullptr;

public:
    Ball(float x, float y, EnvironmentManager* env);
    ~Ball() override = default;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    void resetPosition(float x, float y) override;

    // A custom function to apply force to the ball
    void kick(sf::Vector3f force);

    void snapToPlayer(sf::Vector2f playerPos);
    bool isGrounded() const;

    void setCarrier(Footballer* player);
    Footballer* getCarrier() const;
    Team getPossessionTeam() const;

    void setIntendedReceiver(Footballer* player) { intendedReceiver = player; }
    Footballer* getIntendedReceiver() const { return intendedReceiver; }
};