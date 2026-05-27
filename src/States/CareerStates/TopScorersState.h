#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class TopScorersState : public GameState {
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
    TopScorersState(Game* game, std::shared_ptr<CareerData> careerData);
    ~TopScorersState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};