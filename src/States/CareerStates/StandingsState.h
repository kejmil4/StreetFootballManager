#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

/**
 * @class StandingsState
 * @brief The League Table leaderboard screen.
 * Displays the current rankings of all teams in the campaign, updating dynamically
 * as the season progresses. Assumes that the CareerData's league table has
 * already been mathematically sorted prior to loading this screen.
 */

class StandingsState : public GameState {
private:
    std::shared_ptr<CareerData> career;

    sf::Font font;
    sf::Text titleText;
    sf::Text headerText;
    std::vector<sf::Text> rowTexts;
    sf::Text exitText;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    void buildTable();

public:
    StandingsState(Game* game, std::shared_ptr<CareerData> careerData);
    ~StandingsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};