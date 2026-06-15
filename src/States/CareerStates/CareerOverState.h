#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

/**
 * @class CareerOverState
 * @brief The final summary screen at the conclusion of a campaign.
 * Displays the definitive league table, crowns the champion, and actively
 * deletes the save file from the hard drive to enforce a single-season structure.
 */

class CareerOverState : public GameState {
private:
    std::shared_ptr<CareerData> career;
    sf::Font font;

    sf::Text titleText;
    sf::Text resultText;
    sf::Text headerText;
    std::vector<sf::Text> rowTexts;
    sf::Text exitText;

    void buildFinalTable();
    void deleteSaveFile();

public:
    CareerOverState(Game* game, std::shared_ptr<CareerData> careerData);
    ~CareerOverState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};