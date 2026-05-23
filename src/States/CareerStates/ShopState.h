#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

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

    int selectedIndex;

    void generateRecruits();
    void refreshUI();

public:
    ShopState(Game* game, std::shared_ptr<CareerData> careerData);
    ~ShopState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};