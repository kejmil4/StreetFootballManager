#include "CareerMenuState.h"
#include "CareerHubState.h"
#include "../MenuState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include "../../Career/CareerData.h"
#include "../../Career/LeagueSimulator.h"
#include "LoadCareerState.h"
#include "CareerSetupState.h"
#include <iostream>
#include <fstream>
#include <filesystem>

CareerMenuState::CareerMenuState(Game* game) : GameState(game), titleText(font), selectedIndex(0) {
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for CareerMenu!\n";
    }

    titleText.setString("CAREER MODE");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    titleText.setPosition({Config::CENTER_X, 200.f});

    std::vector<std::string> optionsText = {"Continue", "New Career", "Load Career", "Back"};
    for (int i = 0; i < optionsText.size(); ++i) {
        sf::Text option(font);
        option.setString(optionsText[i]);
        option.setCharacterSize(50);

        sf::FloatRect bounds = option.getLocalBounds();
        option.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        option.setPosition({Config::CENTER_X, 400.f + (i * 80.f)});

        menuOptions.push_back(option);
    }
}

void CareerMenuState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < menuOptions.size() - 1) selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {
            if (selectedIndex == 0) { // Continue
                std::string fileToLoad = "";

                if (std::filesystem::exists("Saves") && std::filesystem::is_directory("Saves")) {
                    // Track the newest file
                    auto latestTime = std::filesystem::file_time_type::min();

                    for (const auto& entry : std::filesystem::directory_iterator("Saves")) {
                        if (entry.path().extension() == ".txt") {
                            // Compare timestamps!
                            auto ftime = std::filesystem::last_write_time(entry);
                            if (ftime > latestTime) {
                                latestTime = ftime;
                                fileToLoad = entry.path().string();
                            }
                        }
                    }
                }

                if (!fileToLoad.empty()) {
                    auto loadedCareer = std::make_shared<CareerData>();
                    if (loadedCareer->loadFromFile(fileToLoad)) {
                        game->changeState(std::make_unique<CareerHubState>(game, loadedCareer));
                    } else {
                        std::cerr << "Save file corrupted!\n";
                    }
                } else {
                    std::cout << "No save files found in Saves/ directory!\n";
                }
            }
            else if (selectedIndex == 1) { // New Career
                game->changeState(std::make_unique<CareerSetupState>(game));
            }
            else if (selectedIndex == 2) {
                game->changeState(std::make_unique<LoadCareerState>(game));
            }
            else if (selectedIndex == 3) { // Back
                game->changeState(std::make_unique<MenuState>(game));
            }
        }
    }
}

void CareerMenuState::update(float dt) {
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

void CareerMenuState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    for (auto& option : menuOptions) {
        target.draw(option);
    }
}