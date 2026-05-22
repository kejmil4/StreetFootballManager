#pragma once
#include "GameState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include <string>

enum class MenuScreen {
    Main,
    CustomMatch,
    Multiplayer,
    MatchSetup,
    Settings
};

enum class MatchType {
    Solo,
    LocalMultiplayer
};

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

    void refreshSetupText();

    void loadScreen(MenuScreen screen);

public:
    MenuState(Game* game);
    ~MenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};