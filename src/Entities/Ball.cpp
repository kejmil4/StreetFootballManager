#include "Ball.h"
#include "../Core/Config.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Ball::Ball(float x, float y)
    : GameObject(x, y),
      position3D({x, y, 0.f}),
      velocity3D({0.f, 0.f, 0.f}),
      gravity(800.f),       // Pulls the ball down 800 pixels per second squared
      bounceFactor(0.6f),   // Retains 60% of its Z-velocity on bounce
      friction(0.98f),       // Loses 2% of X/Y velocity per frame when rolling
      ballSprite(ballTex),
      shadowSprite(shadowTex)
{

    if (!ballTex.loadFromFile("assets/ball.png")) {
        std::cerr << "FAILED TO LOAD: assets/ball.png\n";
    }
    ballSprite.setTexture(ballTex, true);
    ballSprite.setOrigin({16.f, 16.f});
    ballSprite.setScale({2.f, 2.f});

    if (!shadowTex.loadFromFile("assets/ballShadow.png")) {
        std::cerr << "FAILED TO LOAD: assets/ballShadow.png\n";
    }
    shadowSprite.setTexture(shadowTex, true);
    shadowSprite.setOrigin({16.f, 16.f});
    shadowSprite.setColor(sf::Color(255, 255, 255, 120));
    shadowSprite.setScale({2.f, 2.f});

}

void Ball::update(float dt) {
    // Apply gravity to the Z velocity (falling)
    velocity3D.z -= gravity * dt;

    // Move the ball in 3D space
    position3D += velocity3D * dt;

    // Ground Collision Logic (Bouncing & Rolling)
    if (position3D.z <= 0.f) {
        position3D.z = 0.f; // Snap to ground

        // If falling fast enough, bounce!
        if (velocity3D.z < -50.f) {
            velocity3D.z = -velocity3D.z * bounceFactor;
        } else {
            velocity3D.z = 0.f; // Stop micro-bouncing
        }

        // Apply friction to X and Y ONLY when touching the ground
        velocity3D.x *= friction;
        velocity3D.y *= friction;

        // Hard stop if moving incredibly slowly to prevent endless rolling
        if (std::abs(velocity3D.x) < 5.f) velocity3D.x = 0.f;
        if (std::abs(velocity3D.y) < 5.f) velocity3D.y = 0.f;
    }

    // --- NEW BOUNDARY LOGIC ---
    float leftWall = Config::PITCH_LEFT_X;
    float rightWall = Config::PITCH_RIGHT_X;

    // If the ball is vertically between the goal posts, open the physical walls!
    bool inGoalY = (position3D.y > Config::GOAL_TOP_Y && position3D.y < Config::GOAL_BOTTOM_Y);
    if (inGoalY) {
        leftWall = 10.f;  // The back of the left net
        rightWall = Config::WINDOW_WIDTH - 10.f; // The back of the right net
    }

    float ballMargin = 16.f; // The visual radius of the ball

    // X-Axis
    if (position3D.x <= leftWall + ballMargin) {
        position3D.x = leftWall + ballMargin;
        velocity3D.x = -velocity3D.x * bounceFactor;
    } else if (position3D.x >= rightWall - ballMargin) {
        position3D.x = rightWall - ballMargin;
        velocity3D.x = -velocity3D.x * bounceFactor;
    }

    // Y-Axis (Top and Bottom Walls)
    if (position3D.y <= Config::PITCH_TOP_Y + ballMargin) {
        position3D.y = Config::PITCH_TOP_Y + ballMargin;
        velocity3D.y = -velocity3D.y * bounceFactor;
    } else if (position3D.y >= Config::PITCH_BOTTOM_Y - ballMargin) {
        position3D.y = Config::PITCH_BOTTOM_Y - ballMargin;
        velocity3D.y = -velocity3D.y * bounceFactor;
    }

    position = {position3D.x, position3D.y};
}


void Ball::render(sf::RenderTarget& target) {
    // 1. Draw the shadow glued to the ground (Y axis)
    shadowSprite.setPosition({position3D.x, position3D.y});
    target.draw(shadowSprite);

    // 2. Draw the ball floating in the air based on height (Y minus Z axis!)
    ballSprite.setPosition({position3D.x, position3D.y - position3D.z});
    target.draw(ballSprite);
}


void Ball::kick(sf::Vector3f force) {
    velocity3D += force;
}

void Ball::snapToPlayer(sf::Vector2f playerPos) {
    // Kill all momentum
    velocity3D = {0.f, 0.f, 0.f};

    // FIX: Trust the exact coordinates provided by MatchState!
    position3D.x = playerPos.x;
    position3D.y = playerPos.y;
    position3D.z = 0.f; // Force it to the ground

    position = {position3D.x, position3D.y};
}

bool Ball::isGrounded() const {
    return position3D.z <= 0.f;
}

void Ball::resetPosition(float x, float y) {
    GameObject::resetPosition(x, y);

    position3D = {x, y, 0.f};
    velocity3D = {0.f, 0.f, 0.f};
}