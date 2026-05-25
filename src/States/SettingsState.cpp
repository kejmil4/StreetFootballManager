#include "SettingsState.h"
#include "MenuState.h"
#include "../Core/Game.h"
#include "../Core/Config.h"
#include <iostream>

SettingsState::SettingsState(Game* game) : GameState(game), selectedIndex(0), isBinding(false), titleText(font) {
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "Failed to load font in SettingsState\n";
    }

    titleText.setFont(font);
    titleText.setString("CONTROLS SETTINGS");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setPosition({Config::CENTER_X - 450.f, 50.f});

    // 1. Map all our pointers to the Config variables!
    sf::Keyboard::Key* ptrs[14] = {
        &Config::p1Binds.up, &Config::p1Binds.down, &Config::p1Binds.left, &Config::p1Binds.right,
        &Config::p1Binds.passSwitch, &Config::p1Binds.shootTackle, &Config::p1Binds.lobModifier, // <-- Here

        &Config::p2Binds.up, &Config::p2Binds.down, &Config::p2Binds.left, &Config::p2Binds.right,
        &Config::p2Binds.passSwitch, &Config::p2Binds.shootTackle, &Config::p2Binds.lobModifier  // <-- And here
    };

    // Update the UI labels so the player knows what the buttons actually do!
    std::string names[14] = {
        "P1 Up", "P1 Down", "P1 Left", "P1 Right", "P1 Pass/Switch", "P1 Shoot/Tackle", "P1 Lob Mod",
        "P2 Up", "P2 Down", "P2 Left", "P2 Right", "P2 Pass/Switch", "P2 Shoot/Tackle", "P2 Lob Mod"
    };

    for (int i = 0; i < 14; ++i) {
        bindPointers[i] = ptrs[i];
        bindNames[i] = names[i];
    }

    // 2. Build the Two-Column UI (7 items per column)
    for (int i = 0; i < 15; ++i) { // 14 Binds + 1 Back Button
        sf::Text text(font);
        text.setCharacterSize(35);

        if (i < 7) { // Player 1 (Left Column)
            text.setPosition({100.f, 200.f + (i * 70.f)});
        } else if (i < 14) { // Player 2 (Right Column)
            text.setPosition({1100.f, 200.f + ((i - 7) * 70.f)});
        } else { // Back Button
            text.setPosition({Config::CENTER_X - 300.f, 850.f});
            text.setCharacterSize(50);
        }
        menuOptions.push_back(text);
    }
    refreshUI();
}

void SettingsState::refreshUI() {
    for (int i = 0; i < 14; ++i) {
        std::string text = bindNames[i] + ": ";

        if (isBinding && selectedIndex == i) {
            text += "[PRESS NEW KEY]";
            menuOptions[i].setFillColor(sf::Color::Red);
        } else {
            text += keyToStr(*bindPointers[i]);
            menuOptions[i].setFillColor(selectedIndex == i ? sf::Color::Cyan : sf::Color::White);
        }
        menuOptions[i].setString(text);
    }

    menuOptions[14].setString("Back to Menu");
    menuOptions[14].setFillColor(selectedIndex == 14 ? sf::Color::Cyan : sf::Color::White);
}

void SettingsState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        // --- BINDING MODE LOGIC ---
        if (isBinding) {
            // If they press Escape, cancel. Otherwise, assign the new key!
            if (keyPressed->code != sf::Keyboard::Key::Escape) {
                *bindPointers[selectedIndex] = keyPressed->code;
            }
            isBinding = false;
            refreshUI();
            return;
        }

        // --- NAVIGATION LOGIC ---
        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
            refreshUI();
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < 14) selectedIndex++;
            refreshUI();
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            if (selectedIndex == 14) {
                game->changeState(std::make_unique<MenuState>(game)); // Quit to Menu
            } else {
                isBinding = true; // Enter Binding Mode!
                refreshUI();
            }
        }
    }
}

void SettingsState::update(float dt) {}
void SettingsState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    for (const auto& opt : menuOptions) target.draw(opt);
}

// A Quick helper to turn ugly Enums into nice strings!
std::string SettingsState::keyToStr(sf::Keyboard::Key key) {
    int k = static_cast<int>(key);
    // A to Z
    if (k >= static_cast<int>(sf::Keyboard::Key::A) && k <= static_cast<int>(sf::Keyboard::Key::Z)) {
        return std::string(1, 'A' + (k - static_cast<int>(sf::Keyboard::Key::A)));
    }
    // Numbers
    if (k >= static_cast<int>(sf::Keyboard::Key::Num0) && k <= static_cast<int>(sf::Keyboard::Key::Num9)) {
        return std::string(1, '0' + (k - static_cast<int>(sf::Keyboard::Key::Num0)));
    }
    // Special Keys
    switch (key) {
        case sf::Keyboard::Key::Up: return "Up Arrow";
        case sf::Keyboard::Key::Down: return "Down Arrow";
        case sf::Keyboard::Key::Left: return "Left Arrow";
        case sf::Keyboard::Key::Right: return "Right Arrow";
        case sf::Keyboard::Key::Space: return "Spacebar";
        case sf::Keyboard::Key::Enter: return "Enter";
        case sf::Keyboard::Key::Escape: return "Esc";
        case sf::Keyboard::Key::LShift: return "Left Shift";
        case sf::Keyboard::Key::RShift: return "Right Shift";
        case sf::Keyboard::Key::LControl: return "Left Ctrl";
        case sf::Keyboard::Key::RControl: return "Right Ctrl";
        case sf::Keyboard::Key::LAlt: return "Left Alt";
        case sf::Keyboard::Key::RAlt: return "Right Alt";
        default: return "Key";
    }
}