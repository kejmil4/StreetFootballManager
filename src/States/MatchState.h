#pragma once
#include "GameState.h"
#include "../Entities/GameObject.h"
#include "../Entities/Ball.h"
#include "../UI/HUD.h"
#include "../Managers/Referee.h"
#include "../Managers/TeamManager.h"
#include "../UI/PauseMenu.h"
#include <vector>
#include <memory>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

class EnvironmentManager;

enum class PitchType { Grass, Asphalt, Mud };
enum class WeatherType { Clear, Rain, Snow };
enum class Difficulty { Easy, Medium, Hard };

struct MatchSettings {
    int teamSize = 3;
    int homeHumans = 1;
    int awayHumans = 0;

    PitchType pitch = PitchType::Grass;
    WeatherType weather = WeatherType::Clear;
    Difficulty difficulty = Difficulty::Medium;
    int matchLengthSeconds = 180;
};

class MatchState : public GameState {
private:
    MatchSettings settings;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    Ball* matchBall = nullptr;

    std::unique_ptr<Referee> referee;
    std::unique_ptr<TeamManager> teamManager;
    HUD matchHUD;

    std::unique_ptr<EnvironmentManager> envManager;

    std::unique_ptr<PauseMenu> pauseMenu;

    bool isPaused = false;
    void spawnTeams();

public:
    MatchState(Game* game, const MatchSettings& matchSettings);
    ~MatchState() override;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void handleInput(const sf::Event& event) override;
};