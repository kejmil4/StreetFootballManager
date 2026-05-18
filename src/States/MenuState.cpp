#include "MenuState.h"
#include "../Core/Config.h" // Assuming you have Config::CENTER_X here
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "MatchState.h"
#include "../Core/Game.h"

MenuState::MenuState(Game* game) : GameState(game), titleText(menuFont) {
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
    selectedIndex = 0; // Reset the cursor to the top option
    menuOptions.clear(); // Clear the old screen's buttons

    std::string titleStr;
    std::vector<std::string> optionsText;

    // --- YOUR MENU MAP ---
    if (screen == MenuScreen::Main) {
        titleStr = "RETRO STREET SOCCER";
        optionsText = {"Custom Match", "Career Mode (WIP)", "Exit Game"};
    }
    else if (screen == MenuScreen::CustomMatch) {
        titleStr = "CUSTOM MATCH";
        optionsText = {"Solo vs AI", "Multiplayer (WIP)", "Back"};
    }
    else if (screen == MenuScreen::MatchSetup) {
        titleStr = "MATCH SETUP";
        optionsText = {
            "Difficulty: Normal",
            "Weather: Clear",
            "Length: 3 Min",
            "START MATCH",
            "Back"
        };
    }

    // Set the Title Text and center it near the top
    titleText.setString(titleStr);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    titleText.setPosition({Config::CENTER_X, 200.f});

    // Generate the clickable options
    float startY = 400.f; // Where the first option starts
    float spacingY = 80.f; // Space between each option

    for (size_t i = 0; i < optionsText.size(); ++i) {
        sf::Text option(menuFont);
        option.setString(optionsText[i]);
        option.setCharacterSize(50);

        // Center the text
        sf::FloatRect bounds = option.getLocalBounds();
        option.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        option.setPosition({Config::CENTER_X, startY + (i * spacingY)});

        menuOptions.push_back(option);
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

        // --- SELECTION (ENTER KEY) ---
        else if (keyPress->code == sf::Keyboard::Key::Enter) {

            // 1. MAIN MENU LOGIC
            if (currentScreen == MenuScreen::Main) {
                if (selectedIndex == 0) loadScreen(MenuScreen::CustomMatch);
                else if (selectedIndex == 2) game->closeApplication();
            }

            // 2. CUSTOM MATCH LOGIC (This was missing!)
            else if (currentScreen == MenuScreen::CustomMatch) {
                if (selectedIndex == 0) loadScreen(MenuScreen::MatchSetup);
                else if (selectedIndex == 2) loadScreen(MenuScreen::Main); // Go Back
            }

            // 3. MATCH SETUP LOGIC
            else if (currentScreen == MenuScreen::MatchSetup) {
                if (selectedIndex == 3) {
                    // Launch the game!
                    game->changeState(std::make_unique<MatchState>(game));
                }
                else if (selectedIndex == 4) loadScreen(MenuScreen::CustomMatch); // Go Back
            }
        }
    }
}

void MenuState::update(float dt) {
    // Visual Polish: Highlight the currently selected option!
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        if (i == selectedIndex) {
            menuOptions[i].setFillColor(sf::Color::Cyan); // Highlighted
            menuOptions[i].setScale({1.1f, 1.1f});        // Make it slightly bigger
        } else {
            menuOptions[i].setFillColor(sf::Color::White); // Normal
            menuOptions[i].setScale({1.0f, 1.0f});
        }
    }
}

void MenuState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    for (auto& option : menuOptions) {
        target.draw(option);
    }
}