#pragma once
#include "GameState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include <string>

/**
 * @enum MenuScreen
 * @brief Represents the various sub-menus within the main menu hierarchy.
 */

enum class MenuScreen {
    Main,
    CustomMatch,
    Multiplayer,
    MatchSetup,
    Settings
};

/**
 * @enum MatchType
 * @brief Defines the core mode of the upcoming match.
 */
enum class MatchType {
    Solo,
    LocalMultiplayer
};

/**
 * @class MenuState
 * @brief Handles main menu navigation, match configuration, and state transitions.
 * Implements a simple vertical-list menu system using SFML Text.
 * Allows users to tweak match parameters (like pitch and weather) before
 * constructing and launching a new MatchState.
 */
class MenuState : public GameState {
private:
    MenuScreen currentScreen;
    MatchType pendingMatchType;
    int selectedIndex;

    sf::Font menuFont;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    int optPitch = 0;   // 0=Grass, 1=Asphalt, 2=Mud
    int optWeather = 0; // 0=Clear, 1=Rain, 2=Snow
    int optDiff = 1;    // 0=Easy, 1=Medium, 2=Hard
    int optTime = 1;    // 0=1min, 1=3min, 2=5min, 3=10min

    /**
     * @brief Visually updates the setup screen text to reflect the current configuration.
     * Called whenever the user cycles left/right on an option.
     */
    void refreshSetupText();

    /**
     * @brief Transitions the UI to a new sub-menu.
     * Clears the current text options and dynamically populates the screen
     * based on the requested MenuScreen.
     */
    void loadScreen(MenuScreen screen);

public:
    MenuState(Game* game);
    ~MenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};