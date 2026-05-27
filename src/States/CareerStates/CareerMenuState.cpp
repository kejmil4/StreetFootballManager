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

CareerMenuState::CareerMenuState(Game* game)
    : GameState(game), titleText(font), selectedIndex(0), bgSprite(bgTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for CareerMenu!\n";
    }

    std::string bgFilePath = "assets/menus/menuCareerMode.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath << " for CareerMenu!\n";
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});



    std::vector<std::string> optionsText = {"Continue", "New Career", "Load Career", "Back"};

    float startX = Config::WINDOW_WIDTH * 0.84f;
    float startY = Config::WINDOW_HEIGHT * 0.20f;
    float spacingY = Config::WINDOW_HEIGHT * 0.195f;

    for (size_t i = 0; i < optionsText.size(); ++i) {
        sf::Text option(font);
        option.setString(optionsText[i]);
        option.setCharacterSize(30);
        option.setFillColor(sf::Color::White);

        sf::FloatRect bounds = option.getLocalBounds();
        option.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        option.setPosition({startX, startY + (static_cast<float>(i) * spacingY)});

        menuOptions.push_back(option);
    }
}

void CareerMenuState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Up && selectedIndex > 0) {
            selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down && selectedIndex < menuOptions.size() - 1) {
            selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter) {
            if (selectedIndex == 0) { // Continue
                std::string fileToLoad = "";
                if (std::filesystem::exists("Saves") && std::filesystem::is_directory("Saves")) {
                    auto latestTime = std::filesystem::file_time_type::min();
                    for (const auto& entry : std::filesystem::directory_iterator("Saves")) {
                        if (entry.path().extension() == ".txt") {
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
                    }
                }
            }
            else if (selectedIndex == 1) { // New Career
                game->changeState(std::make_unique<CareerSetupState>(game));
            }
            else if (selectedIndex == 2) { // Load Career
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

// --- UPDATED RENDER: Draw the background first! ---
void CareerMenuState::render(sf::RenderTarget& target) {
    target.draw(bgSprite); // Draws your texture behind everything else

    target.draw(titleText);
    for (const auto& opt : menuOptions) {
        target.draw(opt);
    }
}