#include "MatchState.h"
#include "../Entities/PlayerControlled.h"
#include "../Entities/Ball.h"
#include "../Entities/Pitch.h"
#include "../Core/Config.h"
#include "../Entities/AIControlled.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <iostream>
#include <cstdlib>


MatchState::MatchState() {
    gameObjects.push_back(std::make_unique<Pitch>());

    // Spawn the Ball
    gameObjects.push_back(std::make_unique<Ball>(Config::CENTER_X, Config::CENTER_Y));
    for (auto& obj : gameObjects) {
        if (auto b = dynamic_cast<Ball*>(obj.get())) { matchBall = b; }
    }

    EntityStats playerStats = {150.f, 50.f, 50.f, 50.f, 100.f};
    EntityStats enemyStats = {80.f, 50.f, 50.f, 50.f, 100.f};

    // HOME TEAM
    gameObjects.push_back(std::make_unique<PlayerControlled>(Config::CENTER_X - 150.f, Config::CENTER_Y, playerStats));
    gameObjects.push_back(std::make_unique<AIControlled>(Config::CENTER_X - 350.f, Config::CENTER_Y - 200.f, playerStats, matchBall, Team::Home, &gameObjects));
    gameObjects.push_back(std::make_unique<AIControlled>(Config::CENTER_X - 350.f, Config::CENTER_Y + 200.f, playerStats, matchBall, Team::Home, &gameObjects));


    // 3. AWAY TEAM (3 Enemy Bots)
    gameObjects.push_back(std::make_unique<AIControlled>(Config::CENTER_X + 250.f, Config::CENTER_Y, enemyStats, matchBall, Team::Away, &gameObjects));
    gameObjects.push_back(std::make_unique<AIControlled>(Config::CENTER_X + 350.f, Config::CENTER_Y - 200.f, enemyStats, matchBall, Team::Away, &gameObjects));
    gameObjects.push_back(std::make_unique<AIControlled>(Config::CENTER_X + 350.f, Config::CENTER_Y + 200.f, enemyStats, matchBall, Team::Away, &gameObjects));
}

void MatchState::update(float dt) {
    // 1. Standard update for all objects
    for (auto& obj : gameObjects) {
        obj->update(dt);
    }

    // 2. Delegate to our new Referee Helper Functions
    checkPossession();
    checkGoals();
    executeAIAutoTackles();
}

void MatchState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        auto key = event.getIf<sf::Event::KeyPressed>()->code;

            // Find the active player
            PlayerControlled* activePlayer = nullptr;
            for (auto& obj : gameObjects) {
                if (auto p = dynamic_cast<PlayerControlled*>(obj.get())) {
                    activePlayer = p;
                    break; // Found him, stop looking
                }
            }
            if (!activePlayer) return;

            // --- SPACEBAR: Shoot or Tackle ---
            if (key == sf::Keyboard::Key::Space) {
                if (ballCarrier == activePlayer) {
                    handlePlayerShooting(activePlayer);
                }
                else if (ballCarrier && ballCarrier != activePlayer) {
                handlePlayerTackling(activePlayer);
                }
            }

            // --- E KEY: Pass ---
            if (key == sf::Keyboard::Key::E) {
                if (ballCarrier == activePlayer) {
                    handlePlayerPassing(activePlayer);
                }
            }
    }

}


void MatchState::render(sf::RenderTarget& target) {
    // Tell every object in the container to draw itself!
    for (auto& obj : gameObjects) {
        obj->render(target);
    }

    matchHUD.render(target, gameObjects);
}

void MatchState::resetPitch() {
    // Strip possession
    if (ballCarrier) {
        ballCarrier->setPossession(false);
        ballCarrier = nullptr;
    }

    // Loop through everything and send it home
    for (auto& obj : gameObjects) {
        if (auto player = dynamic_cast<PlayerControlled*>(obj.get())) {
            // Spawn Home player on the left half
            player->resetPosition(Config::CENTER_X - 300.f, Config::CENTER_Y);
        }
        else if (auto ball = dynamic_cast<Ball*>(obj.get())) {
            // Put ball dead center
            ball->resetPosition(Config::CENTER_X, Config::CENTER_Y);
        }
        // (Later we will reset the AI players here too!)
    }
}

// ==========================================
// REFEREE HELPER FUNCTIONS
// ==========================================

void MatchState::checkPossession() {
    if (!matchBall) return;

    if (ballCarrier == nullptr) {
        if (matchBall->isGrounded()) {
            for (auto& obj : gameObjects) {
                if (auto entity = dynamic_cast<Entity*>(obj.get())) {
                    float dx = matchBall->getPosition().x - entity->getPosition().x;
                    float dy = matchBall->getPosition().y - entity->getPosition().y;

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

void MatchState::checkGoals() {
    if (!matchBall) return;

    float ballX = matchBall->getPosition().x;

    if (ballX < 0.f) {
        awayScore++;
        matchHUD.updateScore(homeScore, awayScore);
        resetPitch();
    }
    else if (ballX > Config::WINDOW_WIDTH) {
        homeScore++;
        matchHUD.updateScore(homeScore, awayScore);
        resetPitch();
    }
}

void MatchState::executeAIAutoTackles() {
    if (!ballCarrier) return;

    for (auto& obj : gameObjects) {
        if (auto ai = dynamic_cast<AIControlled*>(obj.get())) {
            if (ai != ballCarrier && ai->canTackle() && ai->getTeam() != ballCarrier->getTeam()) {
                float dx = ai->getPosition().x - ballCarrier->getPosition().x;
                float dy = ai->getPosition().y - ballCarrier->getPosition().y;

                if (std::hypot(dx, dy) < 40.f) {
                    ai->resetTackleCooldown();

                    float attackScore = (std::rand() % 100) + ai->getStats().tackling;
                    float defenseScore = (std::rand() % 100) + (ballCarrier->getStats().speed * 0.2f);

                    if (attackScore > defenseScore) {
                        std::cout << "AI WON THE TACKLE!\n";
                        ballCarrier->stun(1.5f);
                        ballCarrier->setPossession(false);
                        ballCarrier = nullptr;

                        float randX = (100.f + (std::rand() % 100)) * ((std::rand() % 2 == 0) ? 1.f : -1.f);
                        float randY = (100.f + (std::rand() % 100)) * ((std::rand() % 2 == 0) ? 1.f : -1.f);
                        matchBall->kick({randX, randY, 200.f});

                        return;
                    } else {
                        std::cout << "AI TACKLE FAILED!\n";
                    }
                }
            }
        }
    }
}

// ==========================================
// INPUT HELPER FUNCTIONS
// ==========================================

void MatchState::handlePlayerShooting(PlayerControlled* activePlayer) {
    sf::Vector2f goalPos(Config::WINDOW_WIDTH, Config::CENTER_Y);
    sf::Vector2f playerPos = ballCarrier->getPosition();

    float dx = goalPos.x - playerPos.x;
    float dy = goalPos.y - playerPos.y;
    float distance = std::hypot(dx, dy);

    float dirX = dx / distance;
    float dirY = dy / distance;

    float powerXY = 0.f;
    float powerZ = 0.f;

    if (playerPos.x < Config::CENTER_X) {
        powerXY = 350.f;
        powerZ = 600.f;
    } else {
        powerXY = 650.f;
        powerZ = 200.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dirY -= 0.6f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dirY += 0.6f;

        float newDist = std::hypot(dirX, dirY);
        dirX /= newDist;
        dirY /= newDist;
    }

    matchBall->kick({dirX * powerXY, dirY * powerXY, powerZ});
    ballCarrier->setPossession(false);
    ballCarrier = nullptr;
}

void MatchState::handlePlayerTackling(PlayerControlled* activePlayer) {
    if (!activePlayer->canTackle()) return;

    float dx = activePlayer->getPosition().x - ballCarrier->getPosition().x;
    float dy = activePlayer->getPosition().y - ballCarrier->getPosition().y;

    if (std::hypot(dx, dy) < 45.f && activePlayer->getTeam() != ballCarrier->getTeam()) {
        activePlayer->resetTackleCooldown();

        float attackScore = (std::rand() % 100) + activePlayer->getStats().tackling;
        float defenseScore = (std::rand() % 100) + (ballCarrier->getStats().speed * 0.2f);

        if (attackScore > defenseScore) {
            std::cout << "YOU WON THE TACKLE!\n";
            ballCarrier->stun(1.5f);
            ballCarrier->setPossession(false);
            ballCarrier = nullptr;

            float randX = (100.f + (std::rand() % 100)) * ((std::rand() % 2 == 0) ? 1.f : -1.f);
            float randY = (100.f + (std::rand() % 100)) * ((std::rand() % 2 == 0) ? 1.f : -1.f);
            matchBall->kick({randX, randY, 200.f});
        } else {
            std::cout << "YOUR TACKLE FAILED!\n";
        }
    }
}

void MatchState::handlePlayerPassing(PlayerControlled* activePlayer) {
    Entity* bestTarget = nullptr;
    float closestDist = 99999.f;

    // 1. Find the closest teammate
    for (auto& obj : gameObjects) {
        if (auto ally = dynamic_cast<Entity*>(obj.get())) {
            // Must be on our team, and cannot be ourselves!
            if (ally != activePlayer && ally->getTeam() == activePlayer->getTeam()) {
                float dist = std::hypot(ally->getPosition().x - activePlayer->getPosition().x,
                                        ally->getPosition().y - activePlayer->getPosition().y);
                if (dist < closestDist) {
                    closestDist = dist;
                    bestTarget = ally;
                }
            }
        }
    }

    // 2. Pass them the ball!
    if (bestTarget) {
        float dx = bestTarget->getPosition().x - activePlayer->getPosition().x;
        float dy = bestTarget->getPosition().y - activePlayer->getPosition().y;
        float dist = std::hypot(dx, dy);

        // Normalize the vector and apply pass speed (e.g., 500) and a slight hop (50 Z-power)
        float passSpeed = 500.f;
        matchBall->kick({(dx / dist) * passSpeed, (dy / dist) * passSpeed, 50.f});

        ballCarrier->setPossession(false);
        ballCarrier = nullptr;
    }
}