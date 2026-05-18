#pragma once
#include "GameState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include <string>

// The 3 screens you mapped out
enum class MenuScreen {
    Main,
    CustomMatch,
    MatchSetup
};

class MenuState : public GameState {
private:
    MenuScreen currentScreen;
    int selectedIndex; // Tracks which option the user is hovering over

    sf::Font menuFont;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;

    // Helper to generate the text options for the current screen
    void loadScreen(MenuScreen screen);

public:
    MenuState(Game* game);
    ~MenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};