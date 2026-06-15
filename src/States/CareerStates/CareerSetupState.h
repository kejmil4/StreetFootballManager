#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

/**
 * @class CareerSetupState
 * @brief The "New Campaign Wizard" screen.
 * Guides the user through creating a new team, capturing custom text input for
 * the team name, and selecting starting parameters (Home Pitch, Logo, Playstyle).
 * Once completed, it generates the initial CareerData payload and boots the simulator.
 */

class CareerSetupState : public GameState {
private:
    std::shared_ptr<CareerData> career;
    sf::Font font;

    sf::Text titleText;
    sf::Text nameInputText;
    std::string playerNameString;

    std::vector<sf::Text> menuOptions;
    int selectedIndex;

    int selectedPitch;
    int selectedLogo;
    int selectedArchetype;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    void refreshUI();

public:
    CareerSetupState(Game* game);
    ~CareerSetupState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};