#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class SettingsState : public GameState {
private:
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;

    int selectedIndex;
    bool isBinding;

    // Pointers that point directly to the keys in Config.h!
    sf::Keyboard::Key* bindPointers[14];
    std::string bindNames[14];

    void refreshUI();
    std::string keyToStr(sf::Keyboard::Key key); // Helper to turn Enums into text

public:
    SettingsState(Game* game);
    ~SettingsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};