#include "StandingsState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>
#include <sstream>
#include <iomanip> // For text alignment

StandingsState::StandingsState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), headerText(font), exitText(font)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for StandingsState!\n";
    }

    titleText.setString("STREET LEAGUE STANDINGS");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setPosition({100.f, 50.f});

    // Table Header
    headerText.setString("RANK  TEAM                 PTS   W   D   L   GF   GA   GD");
    headerText.setCharacterSize(35);
    headerText.setFillColor(sf::Color::Cyan);
    headerText.setPosition({100.f, 150.f});

    exitText.setString("Press ENTER to return");
    exitText.setCharacterSize(40);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition({100.f, 850.f});

    buildTable();
}

void StandingsState::buildTable() {
    rowTexts.clear();

    for (int i = 0; i < career->leagueTable.size(); ++i) {
        const auto& team = career->leagueTable[i];

        // Format the row (Note: For perfect alignment, you might want a monospace font,
        // but stringstreams work well enough for retro vibes!)
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%-4d  %-20s %-5d %-3d %-3d %-3d %-4d %-4d %-4d",
                 i + 1, team.name.c_str(), team.points, team.wins, team.draws, team.losses,
                 team.goalsFor, team.goalsAgainst, team.goalDifference());

        sf::Text rowStr(font);
        rowStr.setString(buffer);
        rowStr.setCharacterSize(30);
        rowStr.setPosition({100.f, 220.f + (i * 60.f)});

        // Highlight the player's team in Green!
        if (team.id == 0) {
            rowStr.setFillColor(sf::Color::Green);
        } else {
            rowStr.setFillColor(sf::Color::White);
        }

        rowTexts.push_back(rowStr);
    }
}

void StandingsState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space || keyPressed->code == sf::Keyboard::Key::Escape) {
            game->changeState(std::make_unique<CareerHubState>(game, career));
        }
    }
}

void StandingsState::update(float dt) {}

void StandingsState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    target.draw(headerText);
    for (const auto& row : rowTexts) {
        target.draw(row);
    }
    target.draw(exitText);
}