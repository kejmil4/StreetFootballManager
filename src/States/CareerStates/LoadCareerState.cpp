#include "LoadCareerState.h"
#include "CareerMenuState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include "../../Career/CareerData.h"
#include <filesystem>
#include <iostream>

LoadCareerState::LoadCareerState(Game* game) : GameState(game), selectedIndex(0), titleText(font), bgSprite(bgTexture) {
    font.openFromFile("assets/font.ttf");


    std::string bgFilePath = "assets/menus/menuLoadCareer.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath;
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});

    if (std::filesystem::exists("Saves") && std::filesystem::is_directory("Saves")) {
        for (const auto& entry : std::filesystem::directory_iterator("Saves")) {
            if (entry.path().extension() == ".txt") {
                filePaths.push_back(entry.path().string());

                sf::Text text(font);
                text.setString(entry.path().stem().string());
                text.setCharacterSize(40);
                menuOptions.push_back(text);
            }
        }
    }

    sf::Text backBtn(font);
    backBtn.setString("Back");
    backBtn.setCharacterSize(30);
    menuOptions.push_back(backBtn);

    refreshUI();
}

void LoadCareerState::refreshUI() {
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        sf::FloatRect bounds = menuOptions[i].getLocalBounds();
        menuOptions[i].setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        menuOptions[i].setPosition({Config::CENTER_X, 350.f + (i * 100.f)});
        menuOptions[i].setFillColor(i == selectedIndex ? sf::Color::Cyan : sf::Color::White);
    }
}

void LoadCareerState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Up && selectedIndex > 0) {
            selectedIndex--;
            refreshUI();
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down && selectedIndex < menuOptions.size() - 1) {
            selectedIndex++;
            refreshUI();
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter) {

            // If they clicked the last option ("Back")
            if (selectedIndex == menuOptions.size() - 1) {
                game->changeState(std::make_unique<CareerMenuState>(game));
            }
            // If they clicked an actual save file!
            else {
                auto loadedCareer = std::make_shared<CareerData>();
                if (loadedCareer->loadFromFile(filePaths[selectedIndex])) {
                    game->changeState(std::make_unique<CareerHubState>(game, loadedCareer));
                } else {
                    std::cerr << "Save file corrupted!\n";
                }
            }
        }
    }
}

void LoadCareerState::update(float dt) {}
void LoadCareerState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    for (const auto& opt : menuOptions) target.draw(opt);
}