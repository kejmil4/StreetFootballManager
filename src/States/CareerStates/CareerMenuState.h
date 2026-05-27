#pragma once
#include "../GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class CareerMenuState : public GameState {
private:
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    int selectedIndex;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

public:
    CareerMenuState(Game* game);
    ~CareerMenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};