#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

/**
 * @class ShopState
 * @brief The campaign's economic hub.
 * Allows the player to spend their earned Street Cred to recruit new procedural
 * players. Handles roster size validation, transaction logic, and the random
 * generation of available free agents.
 */
class ShopState : public GameState {
private:
    std::shared_ptr<CareerData> career;

    std::vector<CareerPlayer> availableRecruits;

    sf::Font font;
    sf::Text titleText;
    sf::Text credText;
    sf::Text rosterCountText;

    std::vector<sf::Text> recruitTexts;
    sf::Text exitText;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    int selectedIndex;

    /**
     * @brief Procedurally generates a batch of free agents with randomized
     * stats, names, and mathematically calculated costs.
     */
    void generateRecruits();

    /**
     * @brief Syncs the on-screen UI text to reflect changes in bank balance,
     * roster size, or the removal of a purchased recruit.
     */
    void refreshUI();

public:
    ShopState(Game* game, std::shared_ptr<CareerData> careerData);
    ~ShopState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};