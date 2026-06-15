#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

/**
 * @class CareerHubState
 * @brief The central dashboard for the single-player campaign.
 * Acts as a router to various sub-menus (Standings, Shop, Team Management)
 * and holds the master shared_ptr to the CareerData. When the user is ready
 * to play, this state packages that data and injects it into the MatchState.
 */

class CareerHubState : public GameState {
private:
    std::shared_ptr<CareerData> career;

    sf::Font font;
    sf::Text headerText;
    sf::Text statsText;
    std::vector<sf::Text> menuOptions;
    int selectedIndex;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    sf::Texture logoTexture;
    sf::Sprite logoSprite;

    /**
     * @brief Packages the career data into MatchSettings and launches the MatchState.
     */
    void prepareNextMatch();
    /**
     * @brief Auto-resolves a scheduled match using RPG math instead of physics.
     * Used when the player skips a week or forces a simulation.
     */
    void simulateNextMatch();

public:
    CareerHubState(Game* game, std::shared_ptr<CareerData> careerData);
    ~CareerHubState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};