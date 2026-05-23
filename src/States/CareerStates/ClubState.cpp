#include "ClubState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>

ClubState::ClubState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), instructionText(font), warningText(font), selectedIndex(0), exitText(font)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for ClubState!\n";
    }

    titleText.setString("MY CREW (ROSTER)");
    titleText.setCharacterSize(70);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setPosition({100.f, 50.f});

    instructionText.setString("Press ENTER to toggle Starter/Bench. Max 3 Starters.");
    instructionText.setCharacterSize(40);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition({100.f, 130.f});

    warningText.setCharacterSize(40);
    warningText.setFillColor(sf::Color::Red);
    warningText.setPosition({100.f, 200.f});
    warningText.setString(""); // Empty by default

    exitText.setFont(font);
    exitText.setString("Back to Hub");
    exitText.setCharacterSize(50);
    exitText.setPosition({100.f, 800.f});

    refreshUI();
}

int ClubState::getStarterCount() {
    int count = 0;
    for (const auto& player : career->roster) {
        if (player.isStarter) count++;
    }
    return count;
}

void ClubState::refreshUI() {
    rosterTexts.clear();

    for (int i = 0; i < career->roster.size(); ++i) {
        sf::Text txt(font);
        CareerPlayer& p = career->roster[i];

        std::string status = p.isStarter ? "[STARTER]" : "[BENCH]";
        std::string info = status + " " + p.name + " | Spd:" + std::to_string((int)p.stats.speed) +
                           " Sht:" + std::to_string((int)p.stats.shooting);

        txt.setString(info);
        txt.setCharacterSize(45);
        txt.setPosition({100.f, 300.f + (i * 80.f)});

        // Color code them so it's easy to read!
        if (p.isStarter) txt.setFillColor(sf::Color::Green);
        else txt.setFillColor(sf::Color(150, 150, 150)); // Gray for benched

        rosterTexts.push_back(txt);
    }
}

void ClubState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        warningText.setString(""); // Clear warning on any key press
        int maxIndex = career->roster.size();

        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < maxIndex) selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {

            // If they clicked a player
            if (selectedIndex < career->roster.size()) {
                CareerPlayer& selectedPlayer = career->roster[selectedIndex];

                if (selectedPlayer.isStarter) {
                    // Always allow benching someone
                    selectedPlayer.isStarter = false;
                } else {
                    // Only allow promoting to starter if we have less than 3
                    if (getStarterCount() < 3) {
                        selectedPlayer.isStarter = true;
                    } else {
                        warningText.setString("WARNING: You already have 3 Starters selected!");
                    }
                }
                refreshUI();
            }
            // If they clicked "Exit"
            else {
                game->changeState(std::make_unique<CareerHubState>(game, career));
            }
        }
    }
}

void ClubState::update(float dt) {
    for (size_t i = 0; i < rosterTexts.size(); ++i) {
        if (i == selectedIndex) {
            rosterTexts[i].setOutlineThickness(3.f);
            rosterTexts[i].setOutlineColor(sf::Color::Cyan);
        } else {
            rosterTexts[i].setOutlineThickness(0.f);
        }
    }

    exitText.setOutlineThickness((selectedIndex == career->roster.size()) ? 3.f : 0.f);
    exitText.setOutlineColor(sf::Color::Cyan);
}

void ClubState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    target.draw(instructionText);
    target.draw(warningText);

    for (const auto& txt : rosterTexts) {
        target.draw(txt);
    }
    target.draw(exitText);
}