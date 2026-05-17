#pragma once
#include "GameState.h"
#include "../Entities/GameObject.h"
#include "../Entities/Entity.h"
#include "../Entities/Ball.h"
#include "../Entities/Footballer.h"
#include "../UI/HUD.h"
#include <vector>
#include <memory>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

class MatchState : public GameState {
private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    HUD matchHUD;
    Ball* matchBall = nullptr;
    Entity* ballCarrier = nullptr;

    int homeScore = 0;
    int awayScore = 0;

    void checkPossession();
    void checkGoals();
    void executeAIAutoTackles();

    void handlePlayerShooting(Footballer* activePlayer);
    void handlePlayerTackling(Footballer* activePlayer);
    void handlePlayerPassing(Footballer* activePlayer);
    void switchHumanControl(Footballer* human);
    void resetPitch();

    float aiReactionTimer = 0.f;
    void handleAIPossession(Footballer* aiCarrier);

    bool isPaused = false;
    float matchDuration = 180.f;
    float timeRemaining;

    sf::Font pauseFont;
    sf::Text pauseText;

public:
    MatchState();
    ~MatchState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};