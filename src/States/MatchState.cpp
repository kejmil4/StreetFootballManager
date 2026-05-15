#include "MatchState.h"
#include "../Entities/PlayerControlled.h"
#include "../Entities/Ball.h"
#include "../Entities/Pitch.h"
#include "../Core/Config.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>


MatchState::MatchState() {
    gameObjects.push_back(std::make_unique<Pitch>());

    EntityStats startingStats = {250.f, 50.f, 50.f, 50.f, 100.f};
    gameObjects.push_back(std::make_unique<PlayerControlled>(Config::CENTER_X, Config::CENTER_Y, startingStats));

    gameObjects.push_back(std::make_unique<Ball>(Config::CENTER_X + 50.f, Config::CENTER_Y));

    for (auto& obj : gameObjects) {
        if (auto b = dynamic_cast<Ball*>(obj.get())) {
            matchBall = b;
        }
    }
}

void MatchState::update(float dt) {
    // 1. Standard update for all objects
    for (auto& obj : gameObjects) {
        obj->update(dt);
    }

    // Check for Possession
    if (matchBall) {
        if (ballCarrier == nullptr) {
            if (matchBall->isGrounded()) {
                for (auto& obj : gameObjects) {
                    if (auto entity = dynamic_cast<Entity*>(obj.get())) {
                        float dx = matchBall->getPosition().x - entity->getPosition().x;
                        float dy = matchBall->getPosition().y - entity->getPosition().y;

                        // If an entity touches the ball, they steal it!
                        if (std::hypot(dx, dy) < 25.f) {
                            ballCarrier = entity;
                            ballCarrier->setPossession(true);
                            break;
                        }
                    }
                }
            }
        } else {
            matchBall->snapToPlayer(ballCarrier->getPosition());
        }
    }
}

void MatchState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space) {

            // If the user presses Space AND they are currently holding the ball
            if (ballCarrier != nullptr && dynamic_cast<PlayerControlled*>(ballCarrier)) {

                sf::Vector2f goalPos(Config::WINDOW_WIDTH, Config::CENTER_Y);
                sf::Vector2f playerPos = ballCarrier->getPosition();

                // 2. Calculate the perfect mathematical vector to the goal
                float dx = goalPos.x - playerPos.x;
                float dy = goalPos.y - playerPos.y;
                float distance = std::hypot(dx, dy);

                // Normalize direction (makes the line exactly length 1.0)
                float dirX = dx / distance;
                float dirY = dy / distance;

                // 3. Default Shot Parameters
                float powerXY = 0.f; // Speed across the ground
                float powerZ = 0.f;  // Height of the kick

                // 4. The Half-Pitch Rule
                if (playerPos.x < Config::CENTER_X) {
                    // Own Half: "Clearance Kick" (High in the air, less forward power)
                    powerXY = 350.f;
                    powerZ = 600.f;
                } else {
                    // Enemy Half: "Strike" (Lower, faster, highly accurate)
                    powerXY = 650.f;
                    powerZ = 200.f;

                    // 5. "Soft Aim" User Input
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                        dirY -= 0.6f; // Aim towards the top corner
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                        dirY += 0.6f; // Aim towards the bottom corner
                    }

                    // Re-normalize so the diagonal bending doesn't make the shot faster
                    float newDist = std::hypot(dirX, dirY);
                    dirX /= newDist;
                    dirY /= newDist;
                }

                // 6. Apply the calculated vector to the kick!
                matchBall->kick({dirX * powerXY, dirY * powerXY, powerZ});

                // 7. Relinquish possession!
                ballCarrier->setPossession(false);
                ballCarrier = nullptr;
            }
        }
    }
}


void MatchState::render(sf::RenderTarget& target) {
    // Tell every object in the container to draw itself!
    for (auto& obj : gameObjects) {
        obj->render(target);
    }
}