#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * @class SettingsState
 * @brief Interactive UI screen for configuring local multiplayer keybindings.
 * Uses a 2D grid navigation system (Rows = Actions, Columns = Players).
 * To elegantly apply changes, it maps the UI grid directly to the memory addresses
 * of the global Config variables, allowing seamless key remapping.
 */

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

    // --- Architecture: The Memory Map ---
    // An array of pointers pointing directly to the global Config::p1Binds and p2Binds.
    // This allows us to dynamically overwrite variables without massive switch statements.
    sf::Keyboard::Key* bindPointers[14];
    std::string actionNames[7];

    /**
     * @brief Syncs the UI text with the current internal configuration state
     * and applies highlighting (Yellow for selected, Red for actively binding).
     */
    void refreshUI();

    /**
     * @brief Translates abstract SFML key enums into human-readable strings.
     */
    std::string keyToStr(sf::Keyboard::Key key);

    sf::Texture bgTexture;
    sf::Sprite bgSprite;
public:
    SettingsState(Game* game);
    ~SettingsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};