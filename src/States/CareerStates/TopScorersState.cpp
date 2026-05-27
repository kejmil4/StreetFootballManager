#include "TopScorersState.h"
#include "CareerHubState.h"
#include "../../Career/LeagueSimulator.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>

TopScorersState::TopScorersState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), headerText(font), exitText(font), bgSprite(bgTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf!\n";
    }

    std::string bgFilePath = "assets/menus/menuTopScorer.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath;
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});

    headerText.setString("RANK  PLAYER                  TEAM       GOALS");
    headerText.setCharacterSize(25);
    headerText.setFillColor(sf::Color::Cyan);
    headerText.setPosition({Config::WINDOW_WIDTH * 0.18f, Config::WINDOW_HEIGHT * 0.3f});

    exitText.setString("Press ENTER to return");
    exitText.setCharacterSize(22);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition({Config::WINDOW_WIDTH * 0.18f, Config::WINDOW_HEIGHT * 0.8f});

    buildTable();
}

void TopScorersState::buildTable() {
    rowTexts.clear();

    float startX = Config::WINDOW_WIDTH * 0.5f;
    float startY = Config::WINDOW_HEIGHT * 0.35f;
    float spacingY = Config::WINDOW_HEIGHT * 0.045f;

    // Grab the top 10 scorers from our new algorithm!
    std::vector<ScorerRecord> topScorers = LeagueSimulator::getTopScorers(career, 10);

    for (int i = 0; i < topScorers.size(); ++i) {
        const auto& record = topScorers[i];

        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%-4d  %-23s %-21s %-5d",
                 i + 1, record.playerName.c_str(), record.teamName.c_str(), record.goals);

        sf::Text rowStr(font);
        rowStr.setString(buffer);
        rowStr.setCharacterSize(20);
        sf::FloatRect bounds = rowStr.getLocalBounds();
        rowStr.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        rowStr.setPosition({startX, startY + (static_cast<float>(i) * spacingY)});

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
    target.draw(bgSprite);
    target.draw(headerText);
    for (const auto& row : rowTexts) target.draw(row);
    target.draw(exitText);
}