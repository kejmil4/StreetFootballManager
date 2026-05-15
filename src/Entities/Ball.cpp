#include "Ball.h"
#include "../Core/Config.h"
#include <cmath>
#include <algorithm>

Ball::Ball(float x, float y)
    : GameObject(x, y),
      position3D({x, y, 0.f}),
      velocity3D({0.f, 0.f, 0.f}),
      gravity(800.f),       // Pulls the ball down 800 pixels per second squared
      bounceFactor(0.6f),   // Retains 60% of its Z-velocity on bounce
      friction(0.98f)       // Loses 2% of X/Y velocity per frame when rolling
{
    // --- TEMPORARY VISUAL: Red Square for the Ball ---
    sf::Image tempImage({16, 16}, sf::Color::Red);
    texture.loadFromImage(tempImage);
    sprite.setTextureRect({{0, 0}, {16, 16}});
    sprite.setOrigin({8.f, 8.f});

    // --- THE SHADOW ---
    shadow.setRadius(8.f);
    shadow.setOrigin({8.f, 8.f});
    shadow.setScale({1.0f, 0.5f}); // Flatten the circle into an oval
    shadow.setFillColor(sf::Color(0, 0, 0, 100)); // Semi-transparent black
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

    // --- WALL COLLISION (X and Y Axes) ---
    float ballMargin = 8.f;

    bool underCrossbar = (position3D.z < 120.f);

    bool inGoalArea = (position3D.y > Config::GOAL_TOP_Y && position3D.y < Config::GOAL_BOTTOM_Y) && underCrossbar;

    // X-Axis (Left and Right Walls)
    if (position3D.x <= ballMargin) {
        if (!inGoalArea) { // Bounce off the wall
            position3D.x = ballMargin;
            velocity3D.x = -velocity3D.x * bounceFactor;
        } else if (position3D.x <= -40.f) {
            // THE LEFT NET: Catch the ball so it doesn't roll forever
            position3D.x = -40.f;
            velocity3D.x = 0.f;
        }
    } else if (position3D.x >= Config::WINDOW_WIDTH - ballMargin) {
        if (!inGoalArea) { // Bounce off the wall
            position3D.x = Config::WINDOW_WIDTH - ballMargin;
            velocity3D.x = -velocity3D.x * bounceFactor;
        } else if (position3D.x >= Config::WINDOW_WIDTH + 40.f) {
            // THE RIGHT NET: Catch the ball
            position3D.x = Config::WINDOW_WIDTH + 40.f;
            velocity3D.x = 0.f;
        }
    }

    // Y-Axis (Top and Bottom Walls)
    if (position3D.y <= ballMargin) {
        position3D.y = ballMargin;
        velocity3D.y = -velocity3D.y * bounceFactor;
    } else if (position3D.y >= Config::WINDOW_HEIGHT - ballMargin) {
        position3D.y = Config::WINDOW_HEIGHT - ballMargin;
        velocity3D.y = -velocity3D.y * bounceFactor;
    }
    position = {position3D.x, position3D.y};

    // --- FAKE 3D VISUALS ---

    // Shadow stays strictly on the 2D ground
    shadow.setPosition({position3D.x, position3D.y});

    // Ball Sprite moves UP the screen based on Z height
    sprite.setPosition({position3D.x, position3D.y - position3D.z});

    // Scale ball UP as it gets higher
    float ballScale = 1.0f + (position3D.z / 300.f);
    sprite.setScale({ballScale, ballScale});

    // Scale shadow DOWN (fade away) as ball gets higher
    float shadowScale = std::max(0.2f, 1.0f - (position3D.z / 300.f));
    shadow.setScale({shadowScale, shadowScale * 0.5f}); // keep oval shape
}

void Ball::render(sf::RenderTarget& target) {
    // Draw shadow FIRST so it is under the ball
    target.draw(shadow);
    target.draw(sprite);
}


void Ball::kick(sf::Vector3f force) {
    velocity3D += force;
}

void Ball::snapToPlayer(sf::Vector2f playerPos) {
    // Kill all momentum
    velocity3D = {0.f, 0.f, 0.f};

    // Snap X/Y strictly to the player's feet (slightly to the right of their center)
    position3D.x = playerPos.x + 15.f;
    position3D.y = playerPos.y + 10.f;
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