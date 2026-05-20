#include "MatchState.h"
#include "../Entities/Pitch.h"
#include "../Core/Config.h"
#include "../Managers/Referee.h"
#include "../Managers/TeamManager.h"
#include <iostream>

MatchState::MatchState(Game* game) : GameState(game), pauseText(pauseFont) {
    // 1. Initialize the Systems
    referee = std::make_unique<Referee>();
    teamManager = std::make_unique<TeamManager>();

    // 2. Setup Pause UI
    if (!pauseFont.openFromFile("assets/font.ttf")) {
        std::cerr << "Failed to load pause font\n";
    }
    pauseText.setString("PAUSED");
    pauseText.setCharacterSize(100);
    pauseText.setFillColor(sf::Color::Yellow);
    pauseText.setPosition({800.f, 400.f}); // Adjust to center of your screen

    // 3. Spawn the Pitch and Ball
    gameObjects.push_back(std::make_unique<Pitch>());
    auto ballPtr = std::make_unique<Ball>(Config::CENTER_X, Config::CENTER_Y);
    matchBall = ballPtr.get();

    for (auto& obj : gameObjects) {
        if (auto b = dynamic_cast<Ball*>(obj.get())) {
            matchBall = b;
            break;
        }
    }

    // 4. Spawn the Teams
    EntityStats playerStats = {150.f, 50.f, 50.f, 50.f, 1000.f};
    EntityStats enemyStats = {100.f, 50.f, 50.f, 50.f, 1000.f};

    // HOME TEAM (Left Side)
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X - 100.f, Config::CENTER_Y, playerStats, matchBall, Team::Home, &gameObjects, true));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X - 300.f, Config::CENTER_Y - 300.f, playerStats, matchBall, Team::Home, &gameObjects, false));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X - 300.f, Config::CENTER_Y + 300.f, playerStats, matchBall, Team::Home, &gameObjects, false));

    // AWAY TEAM (Right Side) - All AI
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X + 450.f, Config::CENTER_Y, enemyStats, matchBall, Team::Away, &gameObjects, false));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X + 400.f, Config::CENTER_Y - 300.f, enemyStats, matchBall, Team::Away, &gameObjects, false));
    gameObjects.push_back(std::make_unique<Footballer>(Config::CENTER_X + 400.f, Config::CENTER_Y + 300.f, enemyStats, matchBall, Team::Away, &gameObjects, false));

    gameObjects.push_back(std::move(ballPtr));

    for (auto& obj : gameObjects) {
        if (auto f = dynamic_cast<Footballer*>(obj.get())) {

            // Give every footballer the TeamManager pointer
            f->setTeamManager(teamManager.get());

            if (f->getTeam() == Team::Home && f->getIsHuman()) {
                teamManager->initializeHuman(f);
            }
        }
    }
}

void MatchState::update(float dt) {
    if (isPaused) return;

    // 1. Tick the Match Clock
    if (referee->updateClock(dt)) {
        std::cout << "FULL TIME!\n";
        // game->changeState(new GameOverState(...)); // Transition out when ready
        return;
    }
    matchHUD.updateTimer(referee->getTimeRemaining());

    // 2. Update all Game Objects (The Brains and Humans think for themselves now)
    for (auto& obj : gameObjects) {
        obj->update(dt);
    }

    // 3. The Referee checks for Goals and Out of Bounds
    if (referee->checkGoals(matchBall)) {
        matchHUD.updateScore(referee->getHomeScore(), referee->getAwayScore());
        referee->resetPitch(gameObjects, matchBall);
    }

    // 4. The TeamManager checks if the human just passed the ball or lost it
    teamManager->update(matchBall, gameObjects);
}

void MatchState::render(sf::RenderTarget& target) {
    // Draw the World
    for (auto& obj : gameObjects) {
        obj->render(target);
    }

    // Draw UI
    matchHUD.render(target, gameObjects);
    if (isPaused) {
        target.draw(pauseText);
    }
}

void MatchState::handleInput(const sf::Event& event) {
    // SFML 3.0 Syntax: Ask if the event is a KeyPressed event
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        // Now you access the code directly from the pointer
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            isPaused = !isPaused;
        }
    }
}