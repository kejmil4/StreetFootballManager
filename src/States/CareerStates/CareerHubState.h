#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class CareerHubState : public GameState {
private:
    std::shared_ptr<CareerData> career; // The Brains!

    sf::Font font;
    sf::Text headerText;
    sf::Text statsText;
    std::vector<sf::Text> menuOptions;
    int selectedIndex;

public:
    CareerHubState(Game* game, std::shared_ptr<CareerData> careerData);
    ~CareerHubState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};