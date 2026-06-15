#include "CareerOverState.h"
#include "../MenuState.h"
#include "../../Career/LeagueSimulator.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>
#include <cstdio>

CareerOverState::CareerOverState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), resultText(font), headerText(font), exitText(font)
{
    font.openFromFile("assets/font.ttf");

    LeagueSimulator::sortStandings(career);

    titleText.setString("SEASON OVER");
    titleText.setCharacterSize(80);
    titleText.setPosition({100.f, 50.f});

    if (career->leagueTable[0].id == 0) {
        titleText.setFillColor(sf::Color::Green);
        resultText.setString("YOU ARE THE STREET CHAMPIONS!");
        resultText.setFillColor(sf::Color::Yellow);
    } else {
        titleText.setFillColor(sf::Color::Red);
        resultText.setString("BETTER LUCK NEXT SEASON.");
        resultText.setFillColor(sf::Color::White);
    }
    resultText.setCharacterSize(50);
    resultText.setPosition({100.f, 150.f});

    headerText.setString("FINAL RANKINGS");
    headerText.setCharacterSize(35);
    headerText.setFillColor(sf::Color::Cyan);
    headerText.setPosition({100.f, 250.f});

    exitText.setString("Press ENTER to return to Main Menu");
    exitText.setCharacterSize(40);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition({100.f, 850.f});

    buildFinalTable();
    deleteSaveFile();
}

void CareerOverState::buildFinalTable() {
    rowTexts.clear();
    for (int i = 0; i < career->leagueTable.size(); ++i) {
        const auto& team = career->leagueTable[i];

        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%-4d  %-20s %-5d PTS",
                 i + 1, team.name.c_str(), team.points);

        sf::Text rowStr(font);
        rowStr.setString(buffer);
        rowStr.setCharacterSize(40);
        rowStr.setPosition({100.f, 320.f + (i * 60.f)});

        if (team.id == 0) rowStr.setFillColor(sf::Color::Green);
        else rowStr.setFillColor(sf::Color::White);

        rowTexts.push_back(rowStr);
    }
}

void CareerOverState::deleteSaveFile() {
    std::string fileToDelete = career->getSaveFileName();
    if (std::remove(fileToDelete.c_str()) == 0) {
        std::cout << "Season over. Save file successfully deleted.\n";
    }
}

void CareerOverState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Escape) {
            // Send them all the way back to the main arcade menu
            game->changeState(std::make_unique<MenuState>(game));
        }
    }
}
void CareerOverState::update(float dt) {}
void CareerOverState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    target.draw(resultText);
    target.draw(headerText);
    for (const auto& row : rowTexts) target.draw(row);
    target.draw(exitText);
}