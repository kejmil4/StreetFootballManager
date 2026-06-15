#pragma once
#include "../GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * @class LoadCareerState
 * @brief The Save File Browser screen.
 * Dynamically scans the local "Saves" directory using modern C++17 filesystem logic.
 * It maps human-readable UI text to the actual system file paths, allowing the user
 * to easily resume any previously saved campaign.
 */

class LoadCareerState : public GameState {
private:
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;

    std::vector<std::string> filePaths;

    int selectedIndex;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    void refreshUI();

public:
    LoadCareerState(Game* game);
    ~LoadCareerState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};