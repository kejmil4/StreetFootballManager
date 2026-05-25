#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

class CareerSetupState : public GameState {
private:
    std::shared_ptr<CareerData> career;
    sf::Font font;

    sf::Text titleText;
    sf::Text nameInputText;
    std::string playerNameString;

    std::vector<sf::Text> menuOptions;
    int selectedIndex;

    // Setup Variables
    int selectedPitch;
    int selectedLogo;
    int selectedArchetype;

    void refreshUI();

public:
    CareerSetupState(Game* game);
    ~CareerSetupState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};