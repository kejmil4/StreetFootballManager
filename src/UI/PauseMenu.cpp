#include "PauseMenu.h"
#include "../Core/Config.h"
#include <iostream>

PauseMenu::PauseMenu() : titleText(font), selectedIndex(0) {
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for PauseMenu!\n";
    }

    backgroundOverlay.setSize({static_cast<float>(Config::WINDOW_WIDTH), static_cast<float>(Config::WINDOW_HEIGHT)});
    backgroundOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // 150 Alpha = Semi-transparent

    titleText.setString("PAUSED");
    titleText.setCharacterSize(100);
    titleText.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    titleText.setPosition({Config::CENTER_X, Config::CENTER_Y - 200.f});

    std::vector<std::string> optionsText = {"Resume", "Restart Match", "Quit to Menu"};
    for (int i = 0; i < optionsText.size(); ++i) {
        sf::Text option(font);
        option.setString(optionsText[i]);
        option.setCharacterSize(60);

        sf::FloatRect bounds = option.getLocalBounds();
        option.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        option.setPosition({Config::CENTER_X, Config::CENTER_Y + (i * 100.f)});

        menuOptions.push_back(option);
    }
}

PauseAction PauseMenu::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            return PauseAction::Resume;
        }

        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < menuOptions.size() - 1) selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {
            if (selectedIndex == 0) return PauseAction::Resume;
            if (selectedIndex == 1) return PauseAction::Restart;
            if (selectedIndex == 2) return PauseAction::Quit;
        }
    }
    return PauseAction::None;
}

void PauseMenu::update(float dt) {
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        if (i == selectedIndex) {
            menuOptions[i].setFillColor(sf::Color::Cyan);
            menuOptions[i].setScale({1.1f, 1.1f});
        } else {
            menuOptions[i].setFillColor(sf::Color::White);
            menuOptions[i].setScale({1.0f, 1.0f});
        }
    }
}

void PauseMenu::render(sf::RenderTarget& target) {
    target.draw(backgroundOverlay);
    target.draw(titleText);
    for (auto& option : menuOptions) {
        target.draw(option);
    }
}