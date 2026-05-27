#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class SettingsState : public GameState {
private:
    sf::Font font;
    sf::Text titleText;

    // Static Column Headers
    sf::Text actionHeader;
    sf::Text p1Header;
    sf::Text p2Header;

    // Interactive Column Data Grid (Upgraded to Vectors for SFML 3.0 Font Requirements)
    std::vector<sf::Text> actionLabels;
    std::vector<sf::Text> p1KeyTexts;
    std::vector<sf::Text> p2KeyTexts;
    sf::Text backButton;

    // Grid Navigation Tracking
    int selectedRow; // 0-6 for binds, 7 for Back button
    int selectedCol; // 0 for Player 1, 1 for Player 2
    bool isBinding;

    // Architecture Pointers linked to Config.h
    sf::Keyboard::Key* bindPointers[14];
    std::string actionNames[7];

    void refreshUI();
    std::string keyToStr(sf::Keyboard::Key key); // Safe string conversion helper

    sf::Texture bgTexture;
    sf::Sprite bgSprite;
public:
    SettingsState(Game* game);
    ~SettingsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};