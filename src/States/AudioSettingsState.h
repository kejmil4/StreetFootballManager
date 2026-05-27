#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class AudioSettingsState : public GameState {
private:
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    int selectedIndex;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    void refreshUI();

public:
    AudioSettingsState(Game* game);
    ~AudioSettingsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};