#include "ShopState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

ShopState::ShopState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), credText(font), rosterCountText(font), selectedIndex(0), exitText(font), bgSprite(bgTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for ShopState!\n";
    }

    std::string bgFilePath = "assets/menus/menuRoster.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath;
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});

    exitText.setFont(font);
    exitText.setString("Back to Hub");
    exitText.setCharacterSize(20);
    exitText.setPosition({Config::WINDOW_WIDTH * 0.2f, Config::WINDOW_HEIGHT * 0.8f});

    generateRecruits();
    refreshUI();
}

void ShopState::generateRecruits() {
    std::vector<std::string> randomNames = {"Slick", "Tank", "Ghost", "Viper", "Bullet", "Brick"};

    for (int i = 0; i < 3; ++i) {
        CareerPlayer p;
        p.name = randomNames[rand() % randomNames.size()] + " " + std::to_string(rand() % 99);

        // Randomize stats between 80 and 130
        p.stats.speed = 80.f + (rand() % 50);
        p.stats.shooting = 80.f + (rand() % 50);
        p.stats.passing = 80.f + (rand() % 50);
        p.stats.tackling = 80.f + (rand() % 50);
        p.stats.maxStamina = 80.f + (rand() % 50);

        p.calculateCost();
        availableRecruits.push_back(p);
    }
}

void ShopState::refreshUI() {

    float startX = Config::WINDOW_WIDTH * 0.2f;
    float startY = Config::WINDOW_HEIGHT * 0.43f;
    float spacingY = Config::WINDOW_HEIGHT * 0.08f;

    credText.setString("Wallet: $" + std::to_string(career->streetCred));
    credText.setCharacterSize(25);
    credText.setFillColor(sf::Color::Green);
    credText.setPosition({Config::WINDOW_WIDTH * 0.2f, Config::WINDOW_HEIGHT * 0.3f});

    rosterCountText.setString("Crew Size: " + std::to_string(career->roster.size()) + " / 7");
    rosterCountText.setCharacterSize(25);
    rosterCountText.setFillColor(sf::Color::White);
    rosterCountText.setPosition({Config::WINDOW_WIDTH * 0.2f, Config::WINDOW_HEIGHT * 0.38f});

    recruitTexts.clear();
    for (int i = 0; i < availableRecruits.size(); ++i) {
        sf::Text txt(font);
        CareerPlayer& p = availableRecruits[i];

        std::string info = p.name + " | COST: $" + std::to_string(p.cost) + "\nSpd:" + std::to_string((int)p.stats.speed) +
            " Sht:" + std::to_string((int)p.stats.shooting)  + " Pass:" + std::to_string((int)p.stats.passing)
             + " Tack:" + std::to_string((int)p.stats.tackling) + " Stamina:" + std::to_string((int)p.stats.maxStamina);

        txt.setString(info);
        txt.setCharacterSize(20);
        txt.setPosition({startX, startY + (static_cast<float>(i) * spacingY)});
        recruitTexts.push_back(txt);
    }
}

void ShopState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        int maxIndex = availableRecruits.size();

        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < maxIndex) selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {

            // If they clicked a player
            if (selectedIndex < availableRecruits.size()) {
                CareerPlayer selectedPlayer = availableRecruits[selectedIndex];

                // 1. Check if they have enough money and space
                if (career->streetCred >= selectedPlayer.cost && career->roster.size() < 5) {

                    // 2. Process Transaction
                    career->streetCred -= selectedPlayer.cost;
                    career->roster.push_back(selectedPlayer);

                    // 3. Remove player from shop
                    availableRecruits.erase(availableRecruits.begin() + selectedIndex);

                    // 4. Force selection back to 0 so we don't point out of bounds
                    selectedIndex = 0;

                    career->saveToFile();

                    refreshUI(); // Update the screen to show lower money and new roster size!
                }
            }
            // If they clicked "Exit"
            else {
                game->changeState(std::make_unique<CareerHubState>(game, career));
            }
        }
    }
}

void ShopState::update(float dt) {
    // Highlight the selected option
    for (size_t i = 0; i < recruitTexts.size(); ++i) {
        recruitTexts[i].setFillColor((i == selectedIndex) ? sf::Color::Cyan : sf::Color::White);
    }

    // Highlight the exit button
    exitText.setFillColor((selectedIndex == availableRecruits.size()) ? sf::Color::Cyan : sf::Color::White);
}

void ShopState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    target.draw(credText);
    target.draw(rosterCountText);

    for (const auto& txt : recruitTexts) {
        target.draw(txt);
    }
    target.draw(exitText);
}