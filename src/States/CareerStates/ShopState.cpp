#include "ShopState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

ShopState::ShopState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), credText(font), rosterCountText(font), selectedIndex(0), exitText(font)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for ShopState!\n";
    }

    titleText.setString("THE BLOCK - RECRUITMENT");
    titleText.setCharacterSize(70);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setPosition({100.f, 50.f});

    exitText.setFont(font);
    exitText.setString("Back to Hub");
    exitText.setCharacterSize(50);
    exitText.setPosition({100.f, 800.f});

    generateRecruits();
    refreshUI();
}

void ShopState::generateRecruits() {
    // Generate 3 random street players
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
    // Update Wallet and Roster Size (Assuming max 5 players)
    credText.setString("Wallet: $" + std::to_string(career->streetCred));
    credText.setCharacterSize(50);
    credText.setFillColor(sf::Color::Green);
    credText.setPosition({100.f, 150.f});

    rosterCountText.setString("Crew Size: " + std::to_string(career->roster.size()) + " / 5");
    rosterCountText.setCharacterSize(40);
    rosterCountText.setFillColor(sf::Color::White);
    rosterCountText.setPosition({500.f, 160.f});

    // Generate Text for Recruits
    recruitTexts.clear();
    for (int i = 0; i < availableRecruits.size(); ++i) {
        sf::Text txt(font);
        CareerPlayer& p = availableRecruits[i];

        // Format: "Name | Spd: 100 Sht: 100 | Cost: $250"
        std::string info = p.name + " | Spd:" + std::to_string((int)p.stats.speed) +
                           " Sht:" + std::to_string((int)p.stats.shooting) +
                           " | COST: $" + std::to_string(p.cost);

        txt.setString(info);
        txt.setCharacterSize(40);
        txt.setPosition({100.f, 300.f + (i * 100.f)});
        recruitTexts.push_back(txt);
    }
}

void ShopState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        // Total options = number of recruits + 1 (for the Exit button)
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

                    // 5. Quick Auto-Save! (Same logic as PostMatchState)
                    std::ofstream saveFile("Saves/career_save.txt");
                    if (saveFile.is_open()) {
                        saveFile << career->teamName << "\n" << career->streetCred << "\n" << career->currentWeek << "\n";
                        saveFile.close();
                    }

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
    target.draw(titleText);
    target.draw(credText);
    target.draw(rosterCountText);

    for (const auto& txt : recruitTexts) {
        target.draw(txt);
    }
    target.draw(exitText);
}