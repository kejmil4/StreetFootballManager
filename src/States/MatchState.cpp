#include "MatchState.h"
#include "../Entities/Pitch.h"
#include "../Core/Config.h"
#include "../Managers/Referee.h"
#include "../Managers/TeamManager.h"
#include "GameOverState.h"
#include "../Core/Game.h"
#include <iostream>

MatchState::MatchState(Game* game, const MatchSettings& matchSettings) : GameState(game), pauseText(pauseFont), settings(matchSettings) {
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
    gameObjects.push_back(std::make_unique<Pitch>(settings.pitch));
    auto ballPtr = std::make_unique<Ball>(Config::CENTER_X, Config::CENTER_Y);
    matchBall = ballPtr.get();

    gameObjects.push_back(std::move(ballPtr));

    spawnTeams();

}

void MatchState::update(float dt) {
    if (isPaused) return;

    if (referee->updateClock(dt)) {
        // MATCH IS OVER! Grab the scores and transition states!
        int finalHome = referee->getHomeScore();
        int finalAway = referee->getAwayScore();

        game->changeState(std::make_unique<GameOverState>(game, finalHome, finalAway));
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

void MatchState::spawnTeams() {
    EntityStats playerStats = {100.f, 50.f, 50.f, 50.f, 1000.f};

    // A simple array of Y-offsets to space players out automatically
    std::vector<float> yOffsets = { 0.f, -300.f, 300.f, -150.f, 150.f };

    // --- SPAWN HOME TEAM ---
    for (int i = 0; i < settings.teamSize; ++i) {
        // Only make them human if we haven't hit the requested human count
        bool isHuman = (i < settings.homeHumans);

        float spawnX = Config::CENTER_X - 250.f; // Home side
        float spawnY = Config::CENTER_Y + (i < yOffsets.size() ? yOffsets[i] : 0.f);

        auto player = std::make_unique<Footballer>(spawnX, spawnY, playerStats, matchBall, Team::Home, &gameObjects, isHuman);
        player->setTeamManager(teamManager.get());

        // Only player 1 (the first spawned human) starts actively controlled
        if (isHuman && i == 0) {
            teamManager->initializeHuman(player.get(), ControllerID::Player1);
        }

        gameObjects.push_back(std::move(player));
    }

    // --- SPAWN AWAY TEAM ---
    for (int i = 0; i < settings.teamSize; ++i) {
        bool isHuman = (i < settings.awayHumans);

        float spawnX = Config::CENTER_X + 250.f; // Away side
        float spawnY = Config::CENTER_Y + (i < yOffsets.size() ? yOffsets[i] : 0.f);

        auto player = std::make_unique<Footballer>(spawnX, spawnY, playerStats, matchBall, Team::Away, &gameObjects, isHuman);
        player->setTeamManager(teamManager.get());

        if (isHuman && i == 0) {
            teamManager->initializeHuman(player.get(), ControllerID::Player2);
        }

        gameObjects.push_back(std::move(player));
    }

    // Finally, ensure the ball is pushed back to the end of the draw order so it renders on top
    for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it) {
        if (dynamic_cast<Ball*>(it->get())) {
            auto ballNode = std::move(*it);
            gameObjects.erase(it);
            gameObjects.push_back(std::move(ballNode));
            break;
        }
    }
}