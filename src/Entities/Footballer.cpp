#include "Footballer.h"
#include "../Core/Config.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <iostream>

// --- HELPER FUNCTION: DYNAMIC PALETTE SWAPPER ---
sf::Texture loadCustomKit(const std::string& filepath, sf::Color jerseyColor, sf::Color shortsColor, sf::Color hairColor) {
    sf::Image image;
    if (!image.loadFromFile(filepath)) {
        std::cerr << "Failed to load player sprite sheet!\n";
    }

    for (unsigned int y = 0; y < image.getSize().y; ++y) {
        for (unsigned int x = 0; x < image.getSize().x; ++x) {
            sf::Color pixelColor = image.getPixel({x, y});

            if (pixelColor == sf::Color::Red || pixelColor == sf::Color::Green) {
                // Swap Neon Green (and any accidental red) to the Jersey Color
                image.setPixel({x, y}, jerseyColor);
            }
            else if (pixelColor == sf::Color::Blue) {
                // Swap Neon Blue to Shorts Color
                image.setPixel({x, y}, shortsColor);
            }
            else if (pixelColor == sf::Color::Magenta) {
                // Swap Neon Magenta to Hair Color
                image.setPixel({x, y}, hairColor);
            }
        }
    }

    sf::Texture tex;
    tex.loadFromImage(image);
    return tex;
}

Footballer::Footballer(float x, float y, const EntityStats& baseStats, Ball* ball, Team teamAlignment, const std::vector<std::unique_ptr<GameObject>>* env, bool startsAsHuman)
    : Entity(x, y, baseStats), targetBall(ball), environment(env), isHuman(startsAsHuman), currentState(AIBrainState::ChasingLoose) {

    setTeam(teamAlignment);

    // 1. Pick team colors
    sf::Color jersey, shorts, hair;
    hair = sf::Color(220, 180, 90); // Default Blonde for now (we can randomize later!)

    if (teamAlignment == Team::Home) {
        jersey = sf::Color::White;
        shorts = sf::Color::Blue;
    } else {
        jersey = sf::Color::Red;
        shorts = sf::Color::Black;
    }

    // 2. Load the sprite sheet using our palette swapper!
    // IMPORTANT: Make sure this path points to where you saved footballer4frames.png
    this->texture = loadCustomKit("assets/footballer4frames.png", jersey, shorts, hair);

    sprite.setTexture(this->texture);
    sprite.setTextureRect({{0, 0}, {32, 32}});

    // Center the origin so flipping the sprite works perfectly!
    sprite.setOrigin({16.f, 16.f});
}

void Footballer::makeHuman() {
    isHuman = true;
    sprite.setColor(sf::Color::White); // Visually pop so you know who you are!
}

void Footballer::makeAI() {
    isHuman = false;
    sprite.setColor(sf::Color(150, 150, 150)); // Fade back to teammate color
}

void Footballer::update(float dt) {
    updateCooldowns(dt);
    if (isStunned()) {
        velocity = {0.f, 0.f};
        return;
    }

    velocity = {0.f, 0.f};

    // ==========================================
    // HUMAN CONTROL LOGIC
    // ==========================================
    if (isHuman) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) velocity.y -= stats.speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) velocity.y += stats.speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) velocity.x -= stats.speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) velocity.x += stats.speed;

        // Diagonal normalization
        if (velocity.x != 0.f && velocity.y != 0.f) {
            float length = std::hypot(velocity.x, velocity.y);
            velocity.x = (velocity.x / length) * stats.speed;
            velocity.y = (velocity.y / length) * stats.speed;
        }
    }
    // ==========================================
    // AI CONTROL LOGIC
    // ==========================================
    else {
        // --- 1. DETERMINE STATE ---
        currentState = AIBrainState::ChasingLoose;
        if (environment) {
            for (const auto& obj : *environment) {
                if (auto entity = dynamic_cast<Entity*>(obj.get())) {
                    if (entity->getPossession()) {
                        if (entity == this) {
                            currentState = AIBrainState::OnBallOffense;
                        }
                        else if (entity->getTeam() == this->getTeam()) {
                            currentState = AIBrainState::OffBallOffense;
                        }
                        else {
                            currentState = AIBrainState::Defending;
                        }
                        break;
                    }
                }
            }
        }

        // --- 2. EXECUTE STATE LOGIC ---
        sf::Vector2f targetPos = position;
        bool ignoreFlocking = false; // ADD THIS: Lets the chaser grab the ball without bouncing off friends!

        switch (currentState) {
            case AIBrainState::ChasingLoose:
            case AIBrainState::Defending: {
                if (targetBall) {
                    // 1. Am I the closest player on my team to the ball?
                    bool amIClosest = true;
                    float myDist = std::hypot(targetBall->getPosition().x - position.x, targetBall->getPosition().y - position.y);

                    if (environment) {
                        for (const auto& obj : *environment) {
                            if (auto ally = dynamic_cast<Footballer*>(obj.get())) {
                                if (ally != this && ally->getTeam() == this->getTeam()) {
                                    float allyDist = std::hypot(targetBall->getPosition().x - ally->getPosition().x,
                                                                targetBall->getPosition().y - ally->getPosition().y);
                                    if (allyDist < myDist) {
                                        amIClosest = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    // 2. Execute dynamic context-aware behavior
                    if (amIClosest) {
                        // PRIMARY CHASER: Run straight at the ball!
                        targetPos = targetBall->getPosition();
                        ignoreFlocking = true;
                    } else {
                        // SUPPORT PLAYER: Where is the ball on the pitch?
                        float ballX = targetBall->getPosition().x;
                        float ownGoalX = (getTeam() == Team::Home) ? 0.f : Config::WINDOW_WIDTH;
                        float distToOwnGoal = std::abs(ballX - ownGoalX);

                        // --- ZONE A: DEFENSIVE EMERGENCY (Ball is within 500px of our own goal) ---
                        if (distToOwnGoal < 500.f) {
                            // Don't drop back! Instead, fan out into a passing lane slightly ahead of the ball
                            float forwardDirection = (getTeam() == Team::Home) ? 120.f : -120.f;
                            targetPos.x = ballX + forwardDirection;

                            // Alternate lanes on the Y-axis so teammates don't stack up
                            targetPos.y = targetBall->getPosition().y + ((position.y > targetBall->getPosition().y) ? 150.f : -150.f);
                        }
                        // --- ZONE B: ATTACKING OVERLAP (Ball is within 500px of opponent's goal) ---
                        else if (std::abs(ballX - (Config::WINDOW_WIDTH - ownGoalX)) < 500.f) {
                            // Make an overlapping offensive run deep into the opponent's corners!
                            float enemyGoalX = (getTeam() == Team::Home) ? Config::WINDOW_WIDTH : 0.f;
                            targetPos.x = enemyGoalX - ((getTeam() == Team::Home) ? 50.f : -50.f);
                            targetPos.y = (position.y < Config::CENTER_Y) ? Config::GOAL_TOP_Y - 100.f : Config::GOAL_BOTTOM_Y + 100.f;
                        }
                        // --- ZONE C: MIDFIELD TRANSITION (Standard containment) ---
                        else {
                            if (currentState == AIBrainState::Defending) {
                                // FIX: Anchor their retreat relative to the BALL, not themselves!
                                // Stay 150 pixels behind the ball to act as a containing sweeper line
                                float defensiveBuffer = (getTeam() == Team::Home) ? -150.f : 150.f;
                                targetPos.x = ballX + defensiveBuffer;

                                // Maintain their current lane on the Y axis so they don't bunch up
                                targetPos.y = position.y;
                            } else {
                                // Ball is loose in midfield, stay stationary and hold your ground
                                targetPos = position;
                            }
                        }
                    }
                }
                break;
            }

            case AIBrainState::OnBallOffense: {
                float enemyGoalX = (getTeam() == Team::Home) ? Config::WINDOW_WIDTH : 0.f;
                targetPos.x = enemyGoalX;
                targetPos.y = Config::CENTER_Y;
                break;
            }

            case AIBrainState::OffBallOffense: {
                float enemyGoalX = (getTeam() == Team::Home) ? Config::WINDOW_WIDTH : 0.f;
                targetPos.x = position.x + ((getTeam() == Team::Home) ? 200.f : -200.f);
                if (targetPos.x > Config::WINDOW_WIDTH - 200.f) targetPos.x = Config::WINDOW_WIDTH - 200.f;
                if (targetPos.x < 200.f) targetPos.x = 200.f;
                targetPos.y = position.y;
                break;
            }
        }

        // --- 3. MOVEMENT ---
        float dx = targetPos.x - position.x;
        float dy = targetPos.y - position.y;
        float distance = std::hypot(dx, dy);

        if (distance > 5.f) {
            velocity.x = (dx / distance) * stats.speed;
            velocity.y = (dy / distance) * stats.speed;
        }

        // --- 4. SEPARATION FLOCKING ---
        // ONLY apply flocking if we aren't the primary chaser!
        if (environment && !ignoreFlocking) {
            sf::Vector2f separationForce(0.f, 0.f);
            int neighbors = 0;
            for (const auto& obj : *environment) {
                if (auto ally = dynamic_cast<Entity*>(obj.get())) {
                    if (ally != this && ally->getTeam() == this->getTeam()) {
                        float distToAlly = std::hypot(ally->getPosition().x - position.x, ally->getPosition().y - position.y);
                        if (distToAlly > 0.1f && distToAlly < 60.f) {
                            separationForce.x += (position.x - ally->getPosition().x) / distToAlly;
                            separationForce.y += (position.y - ally->getPosition().y) / distToAlly;
                            neighbors++;
                        }
                    }
                }
            }
            if (neighbors > 0) {
                velocity.x += (separationForce.x / neighbors) * 150.f;
                velocity.y += (separationForce.y / neighbors) * 150.f;
            }
        }
    }

    animate(dt);

    applyMovement(dt);

}

void Footballer::animate(float dt) {
    // 1. Are we running or standing still?
    bool isMoving = (velocity.x != 0.f || velocity.y != 0.f);

    if (isMoving) {
        anim.timer += dt;
        if (anim.timer >= anim.frameTime) {
            anim.timer = 0.f;
            anim.currentFrame = (anim.currentFrame + 1) % anim.frameCount;
        }
    } else {
        anim.currentFrame = 0; // If standing still, lock to Frame 0 (Neutral)
        anim.timer = 0.f;
    }

    // 2. Slice the correct frame out of the sprite sheet
    sprite.setTextureRect({{anim.currentFrame * 32, 0}, {32, 32}});

    // 3. Flip AND Scale the sprite visually!
    if (velocity.x < 0.f) {
        sprite.setScale({-4.f, 4.f}); // FIX: Changed 1.f to 4.f!
        facingDirection = -1.f;
    }
    else if (velocity.x > 0.f) {
        sprite.setScale({4.f, 4.f});  // FIX: Changed 1.f to 4.f!
        facingDirection = 1.f;
    }
    else {
        // Stationary
        if (getTeam() == Team::Away) {
            sprite.setScale({-4.f, 4.f}); // FIX: Changed 1.f to 4.f!
            facingDirection = -1.f;
        } else {
            sprite.setScale({4.f, 4.f});  // FIX: Changed 1.f to 4.f!
            facingDirection = 1.f;
        }
    }

    // Optional: Make the player you control pop a bit brighter, or darken AI slightly
    if (isHuman) sprite.setColor(sf::Color::White);
    else sprite.setColor(sf::Color(200, 200, 200));


}

