#include "MenuState.h"
#include "../Core/Config.h" // Assuming you have Config::CENTER_X here
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "MatchState.h"
#include "../Core/Game.h"

MenuState::MenuState(Game* game) : GameState(game), titleText(menuFont), bgSprite(bgTexture) {
    if (!menuFont.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for Menu!\n";
    }

    // Initialize the Title Text styling
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::Yellow);

    // Boot up the first screen!
    loadScreen(MenuScreen::Main);
}

void MenuState::loadScreen(MenuScreen screen) {
    currentScreen = screen;
    selectedIndex = 0;
    menuOptions.clear();

    std::string titleStr;
    std::vector<std::string> optionsText;
    std::string bgFilePath;

    float startX = Config::CENTER_X;
    float startY = 400.f;
    float spacingY = 80.f;

    // --- SCREEN SETUP ---
    if (screen == MenuScreen::Main) {
        titleStr = "";
        optionsText = {"Custom Match", "Career Mode", "Settings", "Exit Game"};
        bgFilePath = "assets/menu.png";

        startX = Config::WINDOW_WIDTH * 0.835f;
        startY = Config::WINDOW_HEIGHT * 0.285f;
        spacingY = Config::WINDOW_HEIGHT * 0.155f;
    }
    else if (screen == MenuScreen::CustomMatch) {
        titleStr = "CUSTOM MATCH";
        optionsText = {"Solo vs AI", "Multiplayer", "Back"};
        // bgFilePath = "assets/custom_bg.png";
    }
    else if (screen == MenuScreen::Multiplayer) {
        titleStr = "MULTIPLAYER";
        optionsText = {"Local Multiplayer", "LAN Multiplayer (WIP)", "Back"};
    }
    else if (screen == MenuScreen::Settings) {
        titleStr = "SETTINGS";
        optionsText = {"Controls (WIP)", "Back"};
    }
    else if (screen == MenuScreen::MatchSetup) {
        titleStr = (pendingMatchType == MatchType::Solo) ? "SOLO SETUP" : "LOCAL MULTIPLAYER SETUP";

        optionsText = {"Pitch", "Weather", "Difficulty", "Time", "Launch Game!", "Back"};
    }

    if (!bgFilePath.empty()) {
        if (!bgTexture.loadFromFile(bgFilePath)) {
            std::cerr << "Failed to load background: " << bgFilePath << "\n";
        } else {
            bgSprite.setTexture(bgTexture, true);
            bgSprite.setScale({Config::WINDOW_WIDTH / bgSprite.getLocalBounds().size.x,
                               Config::WINDOW_HEIGHT / bgSprite.getLocalBounds().size.y});
        }
    } else {
        bgTexture = sf::Texture();
        bgSprite.setTexture(bgTexture, true);
    }

    for (size_t i = 0; i < optionsText.size(); ++i) {
        sf::Text option(menuFont);
        option.setString(optionsText[i]);

        if (screen == MenuScreen::Main) {
            option.setCharacterSize(28);
        } else {
            option.setCharacterSize(50);
        }

        sf::FloatRect bounds = option.getLocalBounds();
        option.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});

        option.setPosition({startX, startY + (i * spacingY)});

        menuOptions.push_back(option);
    }
    if (screen == MenuScreen::MatchSetup) {
        refreshSetupText();
    }
}

void MenuState::handleInput(const sf::Event& event) {
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) {

        // --- NAVIGATION ---
        if (keyPress->code == sf::Keyboard::Key::Up) {
            selectedIndex--;
            if (selectedIndex < 0) selectedIndex = menuOptions.size() - 1;
        }
        else if (keyPress->code == sf::Keyboard::Key::Down) {
            selectedIndex++;
            if (selectedIndex >= menuOptions.size()) selectedIndex = 0;
        }

        if (currentScreen == MenuScreen::MatchSetup) {
            if (keyPress->code == sf::Keyboard::Key::Left) {
                if (selectedIndex == 0) optPitch = (optPitch - 1 + 3) % 3;
                else if (selectedIndex == 1) optWeather = (optWeather - 1 + 3) % 3;
                else if (selectedIndex == 2) optDiff = (optDiff - 1 + 3) % 3;
                else if (selectedIndex == 3) optTime = (optTime - 1 + 4) % 4; // Time has 4 options
                refreshSetupText(); // Instantly update the visuals
            }
            else if (keyPress->code == sf::Keyboard::Key::Right) {
                if (selectedIndex == 0) optPitch = (optPitch + 1) % 3;
                else if (selectedIndex == 1) optWeather = (optWeather + 1) % 3;
                else if (selectedIndex == 2) optDiff = (optDiff + 1) % 3;
                else if (selectedIndex == 3) optTime = (optTime + 1) % 4;
                refreshSetupText();
            }
        }

        if (keyPress->code == sf::Keyboard::Key::Enter) {

            // 1. MAIN MENU
            if (currentScreen == MenuScreen::Main) {
                if (selectedIndex == 0) loadScreen(MenuScreen::CustomMatch);
                else if (selectedIndex == 2) loadScreen(MenuScreen::Settings);
                else if (selectedIndex == 3) game->closeApplication();
            }
            // 2. CUSTOM MATCH MENU
            else if (currentScreen == MenuScreen::CustomMatch) {
                if (selectedIndex == 0) {
                    pendingMatchType = MatchType::Solo; // Remember choice!
                    loadScreen(MenuScreen::MatchSetup);
                }
                else if (selectedIndex == 1) loadScreen(MenuScreen::Multiplayer);
                else if (selectedIndex == 2) loadScreen(MenuScreen::Main);
            }
            // 3. MULTIPLAYER MENU
            else if (currentScreen == MenuScreen::Multiplayer) {
                if (selectedIndex == 0) {
                    pendingMatchType = MatchType::LocalMultiplayer; // Remember choice!
                    loadScreen(MenuScreen::MatchSetup);
                }
                else if (selectedIndex == 2) loadScreen(MenuScreen::CustomMatch);
            }
            // 4. SETTINGS MENU
            else if (currentScreen == MenuScreen::Settings) {
                if (selectedIndex == 1) loadScreen(MenuScreen::Main);
            }
            // 5. MATCH SETUP (LAUNCH!)
            else if (currentScreen == MenuScreen::MatchSetup) {
                if (selectedIndex == 4) { // "Launch Game!"

                    MatchSettings settings;

                    settings.pitch = static_cast<PitchType>(optPitch);
                    settings.weather = static_cast<WeatherType>(optWeather);
                    settings.difficulty = static_cast<Difficulty>(optDiff);

                    if (optTime == 0) settings.matchLengthSeconds = 60;
                    else if (optTime == 1) settings.matchLengthSeconds = 180;
                    else if (optTime == 2) settings.matchLengthSeconds = 300;
                    else if (optTime == 3) settings.matchLengthSeconds = 600;

                    if (pendingMatchType == MatchType::Solo) {
                        settings.homeHumans = 1;
                        settings.awayHumans = 0;
                    } else if (pendingMatchType == MatchType::LocalMultiplayer) {
                        settings.homeHumans = 1;
                        settings.awayHumans = 1;
                    }

                    game->changeState(std::make_unique<MatchState>(game, settings));
                }
                else if (selectedIndex == 5) { // "Back"
                    if (pendingMatchType == MatchType::Solo) loadScreen(MenuScreen::CustomMatch);
                    else loadScreen(MenuScreen::Multiplayer);
                }
            }
        }
    }
}

void MenuState::update(float dt) {
    // Visual Polish: Highlight the currently selected option!
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        if (i == selectedIndex) {
            menuOptions[i].setFillColor(sf::Color::Cyan); // Highlighted
            menuOptions[i].setScale({1.1f, 1.1f});
        } else {
            menuOptions[i].setFillColor(sf::Color::White); // Normal
            menuOptions[i].setScale({1.0f, 1.0f});
        }
    }
}

void MenuState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    for (auto& option : menuOptions) {
        target.draw(option);
    }
}

void MenuState::refreshSetupText() {
    if (currentScreen != MenuScreen::MatchSetup) return;

    std::vector<std::string> pitches = {"< Pitch: Grass >", "< Pitch: Asphalt >", "< Pitch: Mud >"};
    std::vector<std::string> weathers = {"< Weather: Clear >", "< Weather: Rain >", "< Weather: Snow >"};
    std::vector<std::string> diffs = {"< Difficulty: Easy >", "< Difficulty: Medium >", "< Difficulty: Hard >"};
    std::vector<std::string> times = {"< Time: 1 Min >", "< Time: 3 Mins >", "< Time: 5 Mins >", "< Time: 10 Mins >"};

    menuOptions[0].setString(pitches[optPitch]);
    menuOptions[1].setString(weathers[optWeather]);

    if (pendingMatchType == MatchType::Solo) {
        menuOptions[2].setString(diffs[optDiff]);
        menuOptions[3].setString(times[optTime]);
        menuOptions[4].setString("Launch Game!");
        menuOptions[5].setString("Back");
    } else {
        menuOptions[2].setString(times[optTime]);
        menuOptions[3].setString("Launch Game!");
        menuOptions[4].setString("Back");
    }

    for (int i = 0; i < 4; ++i) {
        sf::FloatRect bounds = menuOptions[i].getLocalBounds();
        menuOptions[i].setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
    }
}