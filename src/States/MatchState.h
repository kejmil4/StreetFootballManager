#pragma once
#include "GameState.h"
#include "../Entities/GameObject.h"
#include "../Entities/Ball.h"
#include "../UI/HUD.h"
#include "../Managers/Referee.h"
#include "../Managers/TeamManager.h"
#include <vector>
#include <memory>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

class MatchState : public GameState {
private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    Ball* matchBall = nullptr;

    std::unique_ptr<Referee> referee;
    std::unique_ptr<TeamManager> teamManager;
    HUD matchHUD;

    bool isPaused = false;
    sf::Font pauseFont;
    sf::Text pauseText;

public:
    MatchState(Game* game);
    ~MatchState() override = default;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void handleInput(const sf::Event& event) override;
};