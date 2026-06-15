#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

/**
 * @class ClubState
 * @brief The Roster Management screen.
 * Allows the user to view their current squad, check player attributes,
 * and actively toggle players between the Starting Lineup and the Bench.
 * Enforces the strict 3-man street football roster limit.
 */

class ClubState : public GameState {
private:
    std::shared_ptr<CareerData> career;

    sf::Font font;
    sf::Text titleText;
    sf::Text instructionText;
    sf::Text warningText;

    std::vector<sf::Text> rosterTexts;
    sf::Text exitText;

    int selectedIndex;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    void refreshUI();
    int getStarterCount();

public:
    ClubState(Game* game, std::shared_ptr<CareerData> careerData);
    ~ClubState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};