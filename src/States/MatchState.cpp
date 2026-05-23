#include "MatchState.h"
#include "../Entities/Pitch.h"
#include "../Core/Config.h"
#include "../Managers/Referee.h"
#include "../Managers/TeamManager.h"
#include "../Managers/EnvironmentManager.h"
#include "GameOverState.h"
#include "../Core/Game.h"
#include "../States/MenuState.h"
#include "../Career/CareerData.h"
#include "CareerStates/PostMatchState.h"
#include <iostream>

MatchState::MatchState(Game* game, const MatchSettings& matchSettings) : GameState(game), settings(matchSettings) {
    referee = std::make_unique<Referee>(static_cast<float>(settings.matchLengthSeconds));
    teamManager = std::make_unique<TeamManager>();

    pauseMenu = std::make_unique<PauseMenu>();

    // 3. Spawn the Pitch and Ball
    gameObjects.push_back(std::make_unique<Pitch>(settings.pitch));
    envManager = std::make_unique<EnvironmentManager>(settings.pitch, settings.weather);

    auto ballPtr = std::make_unique<Ball>(Config::CENTER_X, Config::CENTER_Y, envManager.get());
    matchBall = ballPtr.get();

    gameObjects.push_back(std::move(ballPtr));

    spawnTeams();

}

MatchState::~MatchState() = default;

void MatchState::update(float dt) {
    if (isPaused) {
        pauseMenu->update(dt);
        return;
    }
    if (referee->updateClock(dt)) {
        if (settings.careerSave != nullptr) {
            game->changeState(std::make_unique<PostMatchState>(
                game,
                settings.careerSave,
                referee->getHomeScore(),
                referee->getAwayScore()
            ));
        }
        else {
            game->changeState(std::make_unique<GameOverState>(
                game,
                referee->getHomeScore(),
                referee->getAwayScore()
            ));
        }
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
    envManager->update(dt);
}

void MatchState::render(sf::RenderTarget& target) {
    // Draw the World
    for (auto& obj : gameObjects) {
        obj->render(target);
    }
    envManager->render(target);

    matchHUD.render(target, gameObjects);
    if (isPaused) {
        pauseMenu->render(target);
    }
}

void MatchState::handleInput(const sf::Event& event) {
    if (isPaused) {
        PauseAction action = pauseMenu->handleInput(event);

        if (action == PauseAction::Resume) {
            isPaused = false;
        }
        else if (action == PauseAction::Restart) {
            // Re-loads the exact same state using your saved settings!
            game->changeState(std::make_unique<MatchState>(game, settings));
        }
        else if (action == PauseAction::Quit) {
            game->changeState(std::make_unique<MenuState>(game));
        }
        return;
    }
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        // Now you access the code directly from the pointer
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            isPaused = !isPaused;
        }
    }
}

void MatchState::spawnTeams() {
    EntityStats humanStats = {100.f, 50.f, 50.f, 50.f, 1000.f};

    EntityStats aiStats = humanStats;
    if (settings.difficulty == Difficulty::Easy) {
        aiStats.speed *= 0.8f;
        aiStats.passing *= 0.8f;
        aiStats.maxStamina *= 0.8f;
    } else if (settings.difficulty == Difficulty::Hard) {
        aiStats.speed *= 1.2f;
        aiStats.passing *= 1.2f;
        aiStats.maxStamina *= 1.5f;
    }

    // A simple array of Y-offsets to space players out automatically
    std::vector<float> yOffsets = { 0.f, -300.f, 300.f, -150.f, 150.f };

    std::vector<EntityStats> starterStats;
    if (settings.careerSave != nullptr) {
        for (const auto& p : settings.careerSave->roster) {
            if (p.isStarter) {
                starterStats.push_back(p.stats);
            }
        }
    }

    // SPAWN HOME TEAM

    for (int i = 0; i < settings.teamSize; ++i) {
        bool isHuman = (i < settings.homeHumans);
        EntityStats statsToUse = isHuman ? humanStats : aiStats;

        if (settings.careerSave != nullptr && i < starterStats.size()) {
            statsToUse = starterStats[i];
        }
        float spawnX = Config::CENTER_X - 250.f; // Home side
        float spawnY = Config::CENTER_Y + (i < yOffsets.size() ? yOffsets[i] : 0.f);

        auto player = std::make_unique<Footballer>(spawnX, spawnY, statsToUse, matchBall, Team::Home, &gameObjects, isHuman);
        player->setTeamManager(teamManager.get());

        // Only player 1 (the first spawned human) starts actively controlled
        if (isHuman && i == 0) {
            teamManager->initializeHuman(player.get(), ControllerID::Player1);
        }

        gameObjects.push_back(std::move(player));
    }

    // SPAWN AWAY TEAM
    for (int i = 0; i < settings.teamSize; ++i) {
        bool isHuman = (i < settings.awayHumans);

        EntityStats statsToUse = isHuman ? humanStats : aiStats;

        float spawnX = Config::CENTER_X + 250.f; // Away side
        float spawnY = Config::CENTER_Y + (i < yOffsets.size() ? yOffsets[i] : 0.f);

        auto player = std::make_unique<Footballer>(spawnX, spawnY, statsToUse, matchBall, Team::Away, &gameObjects, isHuman);
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