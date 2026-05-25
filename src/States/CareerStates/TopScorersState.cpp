#include "TopScorersState.h"
#include "CareerHubState.h"
#include "../../Career/LeagueSimulator.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>

TopScorersState::TopScorersState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), headerText(font), exitText(font)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf!\n";
    }

    titleText.setString("GOLDEN BOOT RACE");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setPosition({100.f, 50.f});

    headerText.setString("RANK  PLAYER                  TEAM                  GOALS");
    headerText.setCharacterSize(35);
    headerText.setFillColor(sf::Color::Cyan);
    headerText.setPosition({100.f, 150.f});

    exitText.setString("Press ENTER to return");
    exitText.setCharacterSize(40);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition({100.f, 850.f});

    buildTable();
}

void TopScorersState::buildTable() {
    rowTexts.clear();

    // Grab the top 10 scorers from our new algorithm!
    std::vector<ScorerRecord> topScorers = LeagueSimulator::getTopScorers(career, 10);

    for (int i = 0; i < topScorers.size(); ++i) {
        const auto& record = topScorers[i];

        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%-4d  %-23s %-21s %-5d",
                 i + 1, record.playerName.c_str(), record.teamName.c_str(), record.goals);

        sf::Text rowStr(font);
        rowStr.setString(buffer);
        rowStr.setCharacterSize(30);
        rowStr.setPosition({100.f, 220.f + (i * 60.f)});

        // Highlight your players in green!
        if (record.teamName == career->teamName) {
            rowStr.setFillColor(sf::Color::Green);
        } else {
            rowStr.setFillColor(sf::Color::White);
        }

        rowTexts.push_back(rowStr);
    }
}

void TopScorersState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Escape) {
            game->changeState(std::make_unique<CareerHubState>(game, career));
        }
    }
}
void TopScorersState::update(float dt) {}
void TopScorersState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    target.draw(headerText);
    for (const auto& row : rowTexts) target.draw(row);
    target.draw(exitText);
}