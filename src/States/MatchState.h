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
class CareerData;

// --- Global Match Configuration Enums ---
enum class PitchType { Grass, Asphalt, Mud };
enum class WeatherType { Clear, Rain, Snow };
enum class Difficulty { Easy, Medium, Hard };

/**
 * @struct MatchSettings
 * @brief A data payload used to configure a match before it boots up.
 * This allows both the Custom Match and the Career Mode
 * to use the exact same MatchState, just by passing in different settings.
 */
struct MatchSettings {
    int teamSize = 3;
    int homeHumans = 1;
    int awayHumans = 0;

    PitchType pitch = PitchType::Grass;
    WeatherType weather = WeatherType::Clear;
    Difficulty difficulty = Difficulty::Medium;
    int matchLengthSeconds = 180;

    std::shared_ptr<CareerData> careerSave = nullptr;

    int opponentTeamId = -1;

    int logoId = -1;
};

/**
 * @class MatchState
 * @brief The core simulation loop of the game.
 * Manages the active roster of GameObjects, routes input to the Pause Menu,
 * and ticks the sub-managers (Referee, Environment, Teams) every frame.
 */
class MatchState : public GameState {
private:
    MatchSettings settings;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    Ball* matchBall = nullptr;

    std::unique_ptr<Referee> referee;
    std::unique_ptr<TeamManager> teamManager;
    HUD matchHUD;

    std::unique_ptr<EnvironmentManager> envManager;

    // --- Match Flow Control ---
    std::unique_ptr<PauseMenu> pauseMenu;

    bool isPaused = false;

    /**
     * @brief Parses the MatchSettings and constructs the physical Footballer entities,
     * assigning them to the correct sides and injecting their proper RPG stats.
     */
    void spawnTeams();

public:
    MatchState(Game* game, const MatchSettings& matchSettings);
    ~MatchState() override;

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void handleInput(const sf::Event& event) override;
};