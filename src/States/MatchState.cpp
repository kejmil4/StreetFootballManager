#include "MatchState.h"
#include "../Entities/Ball.h"
#include "../Entities/Pitch.h"
#include "../Core/Config.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <algorithm>

MatchState::MatchState(Game* game) : GameState(game),timeRemaining(matchDuration), pauseText(pauseFont) {
    gameObjects.push_back(std::make_unique<Pitch>());

    // Spawn the Ball
    gameObjects.push_back(std::make_unique<Ball>(Config::CENTER_X, Config::CENTER_Y));
    for (auto& obj : gameObjects) {
        if (auto b = dynamic_cast<Ball*>(obj.get())) { matchBall = b; }
    }

    EntityStats playerStats = {150.f, 50.f, 50.f, 50.f, 1000.f};
    EntityStats enemyStats = {150.f, 50.f, 50.f, 50.f, 1000.f};

    // 2. Spawn HOME TEAM
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X - 150.f, Config::CENTER_Y, playerStats, matchBall, Team::Home, &gameObjects, true));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X - 350.f, Config::CENTER_Y - 200.f, playerStats, matchBall, Team::Home, &gameObjects, false));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X - 350.f, Config::CENTER_Y + 200.f, playerStats, matchBall, Team::Home, &gameObjects, false));

    // 3. Spawn AWAY TEAM
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X + 350.f, Config::CENTER_Y, enemyStats, matchBall, Team::Away, &gameObjects, false));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X + 350.f, Config::CENTER_Y - 200.f, enemyStats, matchBall, Team::Away, &gameObjects, false));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X + 350.f, Config::CENTER_Y + 200.f, enemyStats, matchBall, Team::Away, &gameObjects, false));

    if (!pauseFont.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for Pause Screen!\n";
    }

    pauseText.setString("PAUSED");
    pauseText.setCharacterSize(100);
    pauseText.setFillColor(sf::Color::White);

    // SFML 3 uses .size.x and .size.y for bounding boxes!
    sf::FloatRect bounds = pauseText.getLocalBounds();
    pauseText.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
    pauseText.setPosition({Config::CENTER_X, Config::CENTER_Y});
}


void MatchState::update(float dt) {
    matchHUD.updateTimer(timeRemaining);

    if (isPaused) return;

    timeRemaining -= dt;
    if (timeRemaining <= 0.f) {
        timeRemaining = 0.f;
        // TODO: End the match and go back to menu!
    }

    for (auto& obj : gameObjects) {
        obj->update(dt);
    }
    checkPossession();
    checkGoals();
    executeAIAutoTackles();

    if (aiReactionTimer > 0.f) {
        aiReactionTimer -= dt;
    }

    if (ballCarrier) {
        if (auto ai = dynamic_cast<Footballer*>(ballCarrier)) {
            if (!ai->getIsHuman()) { // If the bot has the ball...
                handleAIPossession(ai);
            }
        }
    }
}

void MatchState::handleInput(const sf::Event& event) {
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) {


        if (keyPress->code == sf::Keyboard::Key::Escape) {
            isPaused = !isPaused;
        }
    }

    if (event.is<sf::Event::KeyPressed>()) {
        auto key = event.getIf<sf::Event::KeyPressed>()->code;

        // Find the active player
        Footballer* activePlayer = nullptr;
        for (auto& obj : gameObjects) {
            if (auto p = dynamic_cast<Footballer*>(obj.get())) {
                if (p->getIsHuman()) {
                    activePlayer = p;
                    break;
                }
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

    std::vector<GameObject*> renderList;
    for (auto& obj : gameObjects) {
        renderList.push_back(obj.get());
    }


    std::sort(renderList.begin(), renderList.end(), [](GameObject* a, GameObject* b) {
        return a->getPosition().y < b->getPosition().y;
    });

    for (auto* obj : renderList) {
        obj->render(target);
    }


    matchHUD.render(target, gameObjects);

    if (isPaused) {
        target.draw(pauseText);
    }
}

void MatchState::resetPitch() {
    if (ballCarrier) {
        ballCarrier->setPossession(false);
        ballCarrier = nullptr;
    }

    // Spread the teams out correctly so they don't dogpile on top of each other!
    int homeCount = 0;
    int awayCount = 0;

    for (auto& obj : gameObjects) {
        if (auto footballer = dynamic_cast<Footballer*>(obj.get())) {
            if (footballer->getTeam() == Team::Home) {
                float yOffset = (homeCount == 0) ? 0.f : (homeCount == 1) ? -200.f : 200.f;
                footballer->resetPosition(Config::CENTER_X - 250.f, Config::CENTER_Y + yOffset);
                homeCount++;
            } else {
                float yOffset = (awayCount == 0) ? 0.f : (awayCount == 1) ? -200.f : 200.f;
                footballer->resetPosition(Config::CENTER_X + 250.f, Config::CENTER_Y + yOffset);
                awayCount++;
            }
        }
        else if (auto ball = dynamic_cast<Ball*>(obj.get())) {
            ball->resetPosition(Config::CENTER_X, Config::CENTER_Y);
        }
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
                if (auto f = dynamic_cast<Footballer*>(obj.get())) {
                    float dx = f->getPosition().x - matchBall->getPosition().x;
                    float dy = f->getPosition().y - matchBall->getPosition().y;;
                    float distance = std::hypot(dx, dy);

                    if (distance < 90.f && f->canTackle()) {
                        ballCarrier = f;
                        f->setPossession(true);

                        f->resetTackleCooldown();

                        if (auto f = dynamic_cast<Footballer*>(ballCarrier)) {
                            if (f->getTeam() == Team::Home) {
                                switchHumanControl(f);
                            }
                        }
                        break;
                    }
                }
            }
        }
    } else {
        float dirX = 1.f;
        if (auto f = dynamic_cast<Footballer*>(ballCarrier)) {
            dirX = f->getFacingDirection();
        }

        matchBall->snapToPlayer({
             ballCarrier->getPosition().x + (dirX * 35.f),
             ballCarrier->getPosition().y + 60.f
         });
    }
}

void MatchState::checkGoals() {
    if (!matchBall) return;
    float ballX = matchBall->getPosition().x;
    float ballY = matchBall->getPosition().y;

    // Check if it crossed the line AND is between the goal posts!
    bool inGoalY = (ballY > Config::GOAL_TOP_Y && ballY < Config::GOAL_BOTTOM_Y);

    if (ballX < Config::PITCH_LEFT_X && inGoalY) {
        awayScore++;
        matchHUD.updateScore(homeScore, awayScore);
        resetPitch();
    }
    else if (ballX > Config::PITCH_RIGHT_X && inGoalY) {
        homeScore++;
        matchHUD.updateScore(homeScore, awayScore);
        resetPitch();
    }
}

void MatchState::executeAIAutoTackles() {
    if (!ballCarrier) return;

    for (auto& obj : gameObjects) {
        if (auto ai = dynamic_cast<Footballer*>(obj.get())) {
            if (ai != ballCarrier && ai->canTackle() && ai->getTeam() != ballCarrier->getTeam()) {
                float dx = ai->getPosition().x - ballCarrier->getPosition().x;
                float dy = ai->getPosition().y - ballCarrier->getPosition().y;

                if (std::hypot(dx, dy) < 70.f) {
                    if (ai->canTackle() && (std::rand() % 100 < 5)) {
                        ai->resetTackleCooldown();

                        float attackScore = (std::rand() % 100) + ai->getStats().tackling;
                        float defenseScore = (std::rand() % 100) + (ballCarrier->getStats().speed * 0.2f);

                        if (attackScore > defenseScore) {
                            ballCarrier->stun(1.5f);
                            ballCarrier->setPossession(false);
                            ballCarrier = nullptr;

                            float randX = (100.f + (std::rand() % 100)) * ((std::rand() % 2 == 0) ? 1.f : -1.f);
                            float randY = (100.f + (std::rand() % 100)) * ((std::rand() % 2 == 0) ? 1.f : -1.f);
                            matchBall->kick({randX, randY, 200.f});
                            if (ai->getTeam() == Team::Home) {
                                switchHumanControl(ai);
                            }

                            return;
                        }

                    }
                }
            }
        }
    }
}

// ==========================================
// INPUT HELPER FUNCTIONS
// ==========================================

void MatchState::handlePlayerShooting(Footballer* activePlayer) {
    if (!matchBall || !activePlayer->getPossession()) return;

    // 1. Where is the enemy goal?
    bool isHomeTeam = (activePlayer->getTeam() == Team::Home);
    float targetX = isHomeTeam ? Config::PITCH_RIGHT_X : Config::PITCH_LEFT_X;
    float targetY = Config::CENTER_Y; // Aiming at the center of the net

    float dx = targetX - activePlayer->getPosition().x;
    float dy = targetY - activePlayer->getPosition().y;

    // Normalize the base direction to a length of 1.0 (dirX, dirY)
    float dist = std::hypot(dx, dy);
    float dirX = dx / dist;
    float dirY = dy / dist;

    // Variables to hold our kick settings
    float powerXY = 0.f;
    float powerZ = 0.f;
    float maxSpreadRadians = 0.0f;

    // 2. Are we in our own half (Clearance) or the enemy half (Shot)?
    bool inOwnHalf = false;
    if (isHomeTeam) {
        inOwnHalf = (activePlayer->getPosition().x < Config::CENTER_X);
    } else {
        inOwnHalf = (activePlayer->getPosition().x > Config::CENTER_X); // Away team defends the right side!
    }

    if (inOwnHalf) {
        // --- CLEARANCE ---
        powerXY = 350.f; // Doesn't go extremely far horizontally
        powerZ = 600.f;  // Goes very high in the air
        maxSpreadRadians = 0.35f; // Massive chaotic spread (~20 degrees)
    } else {
        // --- SHOT ON GOAL ---
        powerXY = 650.f; // Fast, driven shot
        powerZ = 200.f;  // Stays relatively low to the ground
        maxSpreadRadians = 0.05f; // Very accurate

        // Apply your manual aiming with W/S keys!
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dirY -= 0.6f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dirY += 0.6f;

        // Re-normalize the vector after manual aiming so the shot doesn't accidentally gain extra speed
        float newDist = std::hypot(dirX, dirY);
        dirX /= newDist;
        dirY /= newDist;
    }

    // 3. --- APPLY THE CONE OF INACCURACY ---
    // Convert our current target vector back into an angle
    float baseAngle = std::atan2(dirY, dirX);

    // Calculate the random chaotic spread
    float randomSpread = -maxSpreadRadians + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (maxSpreadRadians * 2.f)));
    float finalAngle = baseAngle + randomSpread;

    // 4. Calculate final kick velocities
    float kickX = std::cos(finalAngle) * powerXY;
    float kickY = std::sin(finalAngle) * powerXY;

    // 5. Execute kick!
    matchBall->kick({kickX, kickY, powerZ});

    // Drop possession
    activePlayer->setPossession(false);
    ballCarrier = nullptr;
}

void MatchState::handlePlayerTackling(Footballer* activePlayer) {
    if (!activePlayer->canTackle()) return;

    float dx = activePlayer->getPosition().x - ballCarrier->getPosition().x;
    float dy = activePlayer->getPosition().y - ballCarrier->getPosition().y;

    if (std::hypot(dx, dy) < 120.f && activePlayer->getTeam() != ballCarrier->getTeam()) {
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

void MatchState::handlePlayerPassing(Footballer* activePlayer) {
    Footballer* bestTarget = nullptr;
    float highestPassScore = -99999.f; // We want to maximize this score

    // 1. Get the player's intended passing direction based on WASD input
    sf::Vector2f inputDir(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) inputDir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) inputDir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) inputDir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) inputDir.x += 1.f;

    // If no keys are pressed, default to the direction they are facing (or just look toward the opponent goal)
    if (inputDir.x == 0.f && inputDir.y == 0.f) {
        inputDir.x = (activePlayer->getTeam() == Team::Home) ? 1.f : -1.f;
    } else {
        // Normalize the input vector so its length is exactly 1.0
        float inputLength = std::hypot(inputDir.x, inputDir.y);
        inputDir.x /= inputLength;
        inputDir.y /= inputLength;
    }

    // 2. Scan the field and score each teammate
    for (auto& obj : gameObjects) {
        if (auto ally = dynamic_cast<Footballer*>(obj.get())) {
            // Must be a teammate, and cannot be ourselves!
            if (ally != activePlayer && ally->getTeam() == activePlayer->getTeam()) {

                // Vector pointing from you to this teammate
                float dx = ally->getPosition().x - activePlayer->getPosition().x;
                float dy = ally->getPosition().y - activePlayer->getPosition().y;
                float distance = std::hypot(dx, dy);

                if (distance > 5.f) {
                    // Normalize the teammate direction vector
                    float toAllyX = dx / distance;
                    float toAllyY = dy / distance;

                    // THE DOT PRODUCT: Measures how closely aligned our aim is with this ally
                    // 1.0 = perfect alignment, 0.0 = perpendicular, -1.0 = opposite direction
                    float alignment = (inputDir.x * toAllyX) + (inputDir.y * toAllyY);

                    // Skip players completely behind us unless there are no other options
                    if (alignment < 0.2f) continue;

                    // --- PASS SCORING FORMULA ---
                    // Alignment is super valuable (multiplied by 500 points).
                    // Distance acts as a slight penalty so we don't accidentally pass across the entire world
                    // if a closer player is also perfectly in line.
                    float passScore = (alignment * 500.f) - (distance * 0.3f);

                    if (passScore > highestPassScore) {
                        highestPassScore = passScore;
                        bestTarget = ally;
                    }
                }
            }
        }
    }

    // 3. Execute the pass to our mathematically best target
    if (bestTarget) {
        float dx = bestTarget->getPosition().x - activePlayer->getPosition().x;
        float dy = bestTarget->getPosition().y - activePlayer->getPosition().y;
        float dist = std::hypot(dx, dy);

        float passSpeed = 500.f;
        matchBall->kick({(dx / dist) * passSpeed, (dy / dist) * passSpeed, 50.f});

        ballCarrier->setPossession(false);
        ballCarrier = nullptr;
    }
}

void MatchState::switchHumanControl(Footballer* newHuman) {
    for (auto& obj : gameObjects) {
        if (auto f = dynamic_cast<Footballer*>(obj.get())) {
            if (f->getIsHuman()) {
                f->makeAI();
            }
        }
    }
    if (newHuman) {
        newHuman->makeHuman();
    }
}

void MatchState::handleAIPossession(Footballer* aiCarrier) {
    if (aiReactionTimer > 0.f) return; // Wait for the AI to "think"

    float enemyGoalX = (aiCarrier->getTeam() == Team::Home) ? Config::WINDOW_WIDTH : 0.f;
    float distToGoal = std::abs(enemyGoalX - aiCarrier->getPosition().x);

    // =====================================
    // 1. SHOOTING LOGIC (Are we close enough?)
    // =====================================
    if (distToGoal < 500.f) {
        aiReactionTimer = 1.0f; // Cooldown after action

        float dx = enemyGoalX - aiCarrier->getPosition().x;
        float dy = Config::CENTER_Y - aiCarrier->getPosition().y;
        float dist = std::hypot(dx, dy);

        // Add a tiny bit of random inaccuracy to the shot
        float variance = ((std::rand() % 100) - 50.f) * 0.01f;
        float dirX = dx / dist;
        float dirY = (dy / dist) + variance;

        // Normalize
        float newDist = std::hypot(dirX, dirY);
        dirX /= newDist;
        dirY /= newDist;

        matchBall->kick({dirX * 650.f, dirY * 650.f, 200.f});
        ballCarrier->setPossession(false);
        ballCarrier = nullptr;

        std::cout << "AI TOOK A SHOT!\n";
        return;
    }

    // =====================================
    // 2. PASSING LOGIC (Am I being attacked?)
    // =====================================
    Footballer* closestEnemy = nullptr;
    float closestEnemyDist = 9999.f;

    Footballer* bestPassTarget = nullptr;
    // We want the teammate furthest down the pitch
    float bestPassScore = (aiCarrier->getTeam() == Team::Home) ? -9999.f : 9999.f;

    // Scan the field
    for (auto& obj : gameObjects) {
        if (auto f = dynamic_cast<Footballer*>(obj.get())) {
            if (f != aiCarrier) {
                if (f->getTeam() != aiCarrier->getTeam()) {
                    // It's an enemy! How close are they?
                    float dist = std::hypot(f->getPosition().x - aiCarrier->getPosition().x,
                                            f->getPosition().y - aiCarrier->getPosition().y);
                    if (dist < closestEnemyDist) {
                        closestEnemyDist = dist;
                        closestEnemy = f;
                    }
                } else {
                    // It's a teammate! Who is closest to the enemy goal?
                    if (aiCarrier->getTeam() == Team::Home) {
                        if (f->getPosition().x > bestPassScore) {
                            bestPassScore = f->getPosition().x;
                            bestPassTarget = f;
                        }
                    } else { // Away team runs left
                        if (f->getPosition().x < bestPassScore) {
                            bestPassScore = f->getPosition().x;
                            bestPassTarget = f;
                        }
                    }
                }
            }
        }
    }

    // If an enemy is within 100 pixels, PANIC AND PASS!
    if (closestEnemyDist < 100.f && bestPassTarget) {
        aiReactionTimer = 1.0f; // Cooldown

        float dx = bestPassTarget->getPosition().x - aiCarrier->getPosition().x;
        float dy = bestPassTarget->getPosition().y - aiCarrier->getPosition().y;
        float dist = std::hypot(dx, dy);

        float passSpeed = 500.f;
        matchBall->kick({(dx / dist) * passSpeed, (dy / dist) * passSpeed, 50.f});

        ballCarrier->setPossession(false);
        ballCarrier = nullptr;

        std::cout << "AI PASSED THE BALL UNDER PRESSURE!\n";
    }
}