#include "SettingsState.h"
#include "MenuState.h"
#include "../Core/Game.h"
#include "../Core/Config.h"
#include <iostream>

SettingsState::SettingsState(Game* game)
    : GameState(game), selectedRow(0), selectedCol(0), isBinding(false), titleText(font),
      actionHeader(font), p1Header(font), p2Header(font), backButton(font), bgSprite(bgTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "Failed to load font in SettingsState\n";
    }
    std::string bgFilePath = "assets/menus/menuControlsSettings.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath << " for CareerMenu!\n";
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});

    // 2. Initialize Core Actions Mapping (Row strings)
    actionNames[0] = "Move Up";
    actionNames[1] = "Move Down";
    actionNames[2] = "Move Left";
    actionNames[3] = "Move Right";
    actionNames[4] = "Pass / Player Switch";
    actionNames[5] = "Shoot / Tackle";
    actionNames[6] = "Lob Modifier";

    // Link structural configuration references sequentially
    sf::Keyboard::Key* ptrs[14] = {
        &Config::p1Binds.up, &Config::p1Binds.down, &Config::p1Binds.left, &Config::p1Binds.right,
        &Config::p1Binds.passSwitch, &Config::p1Binds.shootTackle, &Config::p1Binds.lobModifier,

        &Config::p2Binds.up, &Config::p2Binds.down, &Config::p2Binds.left, &Config::p2Binds.right,
        &Config::p2Binds.passSwitch, &Config::p2Binds.shootTackle, &Config::p2Binds.lobModifier
    };
    for (int i = 0; i < 14; ++i) {
        bindPointers[i] = ptrs[i];
    }

    // 3. Header Text Definitions
    actionHeader.setString("ACTION");
    p1Header.setString("PLAYER 1");
    p2Header.setString("PLAYER 2");

    actionHeader.setCharacterSize(25);
    p1Header.setCharacterSize(25);
    p2Header.setCharacterSize(25);

    actionHeader.setFillColor(sf::Color(200, 200, 200));
    p1Header.setFillColor(sf::Color(255, 100, 100)); // Tint red to match P1 arrow motif
    p2Header.setFillColor(sf::Color(100, 180, 255)); // Tint blue to match P2 arrow motif

    // 4. FIX: Safely construct elements passing font reference via emplace_back
    for (int i = 0; i < 7; ++i) {
        actionLabels.emplace_back(font);
        p1KeyTexts.emplace_back(font);
        p2KeyTexts.emplace_back(font);

        actionLabels[i].setCharacterSize(20);
        p1KeyTexts[i].setCharacterSize(20);
        p2KeyTexts[i].setCharacterSize(20);
    }

    backButton.setString("Save & Return to Menu");
    backButton.setCharacterSize(40);

    refreshUI();
}

void SettingsState::refreshUI() {
    float startY = Config::WINDOW_HEIGHT * 0.35f;
    float spacingY = Config::WINDOW_HEIGHT * 0.065f;

    // Track Columns alignment coordinates
    float colActionX = Config::WINDOW_WIDTH * 0.18f;
    float colP1X     = Config::WINDOW_WIDTH * 0.50f;
    float colP2X     = Config::WINDOW_WIDTH * 0.75f;

    // Position Header Track
    float headerY = startY - 65.f;
    actionHeader.setPosition({colActionX, headerY});

    sf::FloatRect p1HBounds = p1Header.getLocalBounds();
    p1Header.setOrigin({p1HBounds.position.x + p1HBounds.size.x / 2.f, p1HBounds.position.y + p1HBounds.size.y / 2.f});
    p1Header.setPosition({colP1X, headerY});

    sf::FloatRect p2HBounds = p2Header.getLocalBounds();
    p2Header.setOrigin({p2HBounds.position.x + p2HBounds.size.x / 2.f, p2HBounds.position.y + p2HBounds.size.y / 2.f});
    p2Header.setPosition({colP2X, headerY});

    // Populate and align grid options entries
    for (int i = 0; i < 7; ++i) {
        float posY = startY + (static_cast<float>(i) * spacingY);

        // Action text (Left aligned, vertically centered)
        actionLabels[i].setString(actionNames[i]);
        sf::FloatRect actBounds = actionLabels[i].getLocalBounds();
        actionLabels[i].setOrigin({0.f, actBounds.position.y + actBounds.size.y / 2.f});
        actionLabels[i].setPosition({colActionX, posY});

        // Player 1 Key texts (Perfect center origin)
        p1KeyTexts[i].setString(keyToStr(*bindPointers[i]));
        sf::FloatRect p1Bounds = p1KeyTexts[i].getLocalBounds();
        p1KeyTexts[i].setOrigin({p1Bounds.position.x + p1Bounds.size.x / 2.f, p1Bounds.position.y + p1Bounds.size.y / 2.f});
        p1KeyTexts[i].setPosition({colP1X, posY});

        // Player 2 Key texts (Perfect center origin)
        p2KeyTexts[i].setString(keyToStr(*bindPointers[i + 7]));
        sf::FloatRect p2Bounds = p2KeyTexts[i].getLocalBounds();
        p2KeyTexts[i].setOrigin({p2Bounds.position.x + p2Bounds.size.x / 2.f, p2Bounds.position.y + p2Bounds.size.y / 2.f});
        p2KeyTexts[i].setPosition({colP2X, posY});
    }

    // Centered Back button at base
    sf::FloatRect backBounds = backButton.getLocalBounds();
    backButton.setOrigin({backBounds.position.x + backBounds.size.x / 2.f, backBounds.position.y + backBounds.size.y / 2.f});
    backButton.setPosition({Config::CENTER_X, startY + (7.2f * spacingY)});
}

void SettingsState::handleInput(const sf::Event& event) {
    // Intercept input assignment phase
    if (isBinding) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            int targetBindIndex = (selectedCol * 7) + selectedRow;
            *bindPointers[targetBindIndex] = keyPressed->code;

            isBinding = false;
            game->getAudio()->playSound("menu_select");
            refreshUI();
            return;
        }
    }

    // Standard Grid Interaction Mode
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Up && selectedRow > 0) {
            selectedRow--;
            game->getAudio()->playSound("menu_move");
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down && selectedRow < 7) {
            selectedRow++;
            game->getAudio()->playSound("menu_move");
        }
        else if (keyPressed->code == sf::Keyboard::Key::Left && selectedRow < 7 && selectedCol > 0) {
            selectedCol = 0;
            game->getAudio()->playSound("menu_move");
        }
        else if (keyPressed->code == sf::Keyboard::Key::Right && selectedRow < 7 && selectedCol < 1) {
            selectedCol = 1;
            game->getAudio()->playSound("menu_move");
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            if (selectedRow == 7) { // Trigger Back Button Execution
                Config::saveSettings();
                game->getAudio()->playSound("menu_select");
                game->changeState(std::make_unique<MenuState>(game));
            } else { // Put active cell into assignment phase
                isBinding = true;
                game->getAudio()->playSound("menu_select");
            }
        }
    }
}

void SettingsState::update(float dt) {
    if (selectedRow == 7) { // Focus explicitly targeting the back selector button
        backButton.setFillColor(sf::Color::Cyan);
        backButton.setScale({1.1f, 1.1f});

        for (int i = 0; i < 7; ++i) {
            actionLabels[i].setFillColor(sf::Color::White);
            p1KeyTexts[i].setFillColor(sf::Color::White);
            p1KeyTexts[i].setScale({1.0f, 1.0f});
            p2KeyTexts[i].setFillColor(sf::Color::White);
            p2KeyTexts[i].setScale({1.0f, 1.0f});
        }
    } else { // Grid element currently possesses cursor interaction priority
        backButton.setFillColor(sf::Color::White);
        backButton.setScale({1.0f, 1.0f});

        for (int i = 0; i < 7; ++i) {
            if (i == selectedRow) {
                actionLabels[i].setFillColor(sf::Color::Yellow); // Highlight action text row track

                // Player 1 Focused Cell Highlights
                if (selectedCol == 0) {
                    p1KeyTexts[i].setFillColor(isBinding ? sf::Color::Magenta : sf::Color::Cyan);
                    p1KeyTexts[i].setScale({1.15f, 1.15f});
                    p2KeyTexts[i].setFillColor(sf::Color(130, 130, 130)); // Dim unfocused cell column
                    p2KeyTexts[i].setScale({1.0f, 1.0f});
                }
                // Player 2 Focused Cell Highlights
                else {
                    p2KeyTexts[i].setFillColor(isBinding ? sf::Color::Magenta : sf::Color::Cyan);
                    p2KeyTexts[i].setScale({1.15f, 1.15f});
                    p1KeyTexts[i].setFillColor(sf::Color(130, 130, 130)); // Dim unfocused cell column
                    p1KeyTexts[i].setScale({1.0f, 1.0f});
                }
            } else {
                actionLabels[i].setFillColor(sf::Color::White);
                p1KeyTexts[i].setFillColor(sf::Color::White);
                p1KeyTexts[i].setScale({1.0f, 1.0f});
                p2KeyTexts[i].setFillColor(sf::Color::White);
                p2KeyTexts[i].setScale({1.0f, 1.0f});
            }
        }
    }
}

void SettingsState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    target.draw(actionHeader);
    target.draw(p1Header);
    target.draw(p2Header);

    for (int i = 0; i < 7; ++i) {
        target.draw(actionLabels[i]);
        target.draw(p1KeyTexts[i]);
        target.draw(p2KeyTexts[i]);
    }
    target.draw(backButton);
}

std::string SettingsState::keyToStr(sf::Keyboard::Key key) {
    int k = static_cast<int>(key);
    if (k >= static_cast<int>(sf::Keyboard::Key::A) && k <= static_cast<int>(sf::Keyboard::Key::Z)) {
        return std::string(1, 'A' + (k - static_cast<int>(sf::Keyboard::Key::A)));
    }
    if (k >= static_cast<int>(sf::Keyboard::Key::Num0) && k <= static_cast<int>(sf::Keyboard::Key::Num9)) {
        return std::string(1, '0' + (k - static_cast<int>(sf::Keyboard::Key::Num0)));
    }
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
        case sf::Keyboard::Key::Tab: return "Tab";
        // FIX: Lowercase spelling for Backspace
        case sf::Keyboard::Key::Backspace: return "Backspace";
        case sf::Keyboard::Key::Period: return ".";
        default: return "Unknown Key";
    }
}