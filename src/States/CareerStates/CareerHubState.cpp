#include "CareerHubState.h"
#include "../MenuState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include "TopScorersState.h"
#include "ClubState.h"
#include "../MatchState.h"
#include "StandingsState.h"
#include "ShopState.h"
#include <iostream>

#include "PostMatchState.h"

CareerHubState::CareerHubState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), headerText(font), statsText(font), selectedIndex(0), bgSprite(bgTexture), logoSprite(logoTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for CareerHub!\n";
    }

    std::string bgFilePath = "assets/menus/menuCareerHub.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath << " for CareerHub!\n";
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});

    int displayLogoId = career->logoId;
    std::string logoPath = "assets/logos/logo" + std::to_string(displayLogoId) + ".png";

    if (!logoTexture.loadFromFile(logoPath)) {
        std::cerr << "FAILED TO LOAD LOGO: " << logoPath << " for CareerHub!\n";
    }
    logoSprite.setTexture(logoTexture, true);

    logoSprite.setScale({0.4f, 0.4f});
    logoSprite.setPosition({Config::WINDOW_WIDTH * 0.4f, Config::WINDOW_HEIGHT * 0.21f});

    // 1. Team Name Header
    headerText.setString(career->teamName);
    headerText.setCharacterSize(30);
    headerText.setFillColor(sf::Color::Yellow);
    headerText.setPosition({Config::WINDOW_WIDTH * 0.5f, Config::WINDOW_HEIGHT * 0.25f});

    // 2. Resources Info
    statsText.setString("Week " + std::to_string(career->currentWeek) + "    Street Cred: $" + std::to_string(career->streetCred));
    statsText.setCharacterSize(30);
    statsText.setFillColor(sf::Color::Green);
    statsText.setPosition({Config::WINDOW_WIDTH * 0.3f, Config::WINDOW_HEIGHT * 0.35f});

    std::string opponentName = "UNKNOWN";
    for (const auto& match : career->schedule) {
        if (match.week == career->currentWeek) {
            if (match.homeTeamId == 0) {
                opponentName = career->leagueTable[match.awayTeamId].name + " (HOME)";
                break;
            } else if (match.awayTeamId == 0) {
                opponentName = career->leagueTable[match.homeTeamId].name + " (AWAY)";
                break;
            }
        }
    }
    // HUB Options
    std::vector<std::string> optionsText = {
        "Play Next Match vs " + opponentName,
        "Simulate Next Match vs " + opponentName,
        "My Crew (Roster)",
        "The Block (Shop)",
        "League Standings",
        "Top Scorer Standings",
        "Save & Quit to Main Menu"
    };

    float startX = Config::WINDOW_WIDTH * 0.5f;
    float startY = Config::WINDOW_HEIGHT * 0.45f;
    float spacingY = Config::WINDOW_HEIGHT * 0.06f;

    for (int i = 0; i < optionsText.size(); ++i) {
        sf::Text option(font);
        option.setString(optionsText[i]);
        option.setCharacterSize(30);

        sf::FloatRect bounds = option.getLocalBounds();
        option.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        option.setPosition({startX, startY + (static_cast<float>(i) * spacingY)});

        menuOptions.push_back(option);
    }

}

void CareerHubState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < menuOptions.size() - 1) selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {
            if (selectedIndex == 0) { // "Play Match"
                prepareNextMatch();
            }
            else if (selectedIndex == 1 ) { // "Simulate Match"
                simulateNextMatch();
            }
            else if (selectedIndex == 2) { // "My Crew"
                game->changeState(std::make_unique<ClubState>(game, career));
            }
            else if (selectedIndex == 3) { // "The Block"
                game->changeState(std::make_unique<ShopState>(game, career));
            }
            else if (selectedIndex == 4) { // "League Standings"
                game->changeState(std::make_unique<StandingsState>(game, career));
            }
            else if (selectedIndex == 5) { // "Top Scorer"
                game->changeState(std::make_unique<TopScorersState>(game, career));
            }
            else if (selectedIndex == 6) {
                career->saveToFile();

                game->changeState(std::make_unique<MenuState>(game));
            }
        }
    }
}

void CareerHubState::update(float dt) {
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        if (i == selectedIndex) {
            menuOptions[i].setFillColor(sf::Color::Cyan);
        } else {
            menuOptions[i].setFillColor(sf::Color::White);
        }
    }
}

void CareerHubState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    target.draw(logoSprite);
    target.draw(headerText);
    target.draw(statsText);
    for (auto& option : menuOptions) {
        target.draw(option);
    }
}

void CareerHubState::prepareNextMatch() {
    int starterCount = 0;
    for (const auto& p : career->roster) {
        if (p.isStarter) starterCount++;
    }

    if (starterCount != 3) {
        std::cout << "ERROR: You must have exactly 3 starters selected!\n";
        return;
    }

    MatchSettings careerSettings;
    careerSettings.teamSize = 3;
    careerSettings.homeHumans = 1;
    careerSettings.awayHumans = 0;
    careerSettings.careerSave = career;

    bool isPlayerHome = false; //

    for (const auto& match : career->schedule) {
        if (match.week == career->currentWeek) {
            if (match.homeTeamId == 0) {
                careerSettings.opponentTeamId = match.awayTeamId;
                isPlayerHome = true;
            } else if (match.awayTeamId == 0) {
                careerSettings.opponentTeamId = match.homeTeamId;
                isPlayerHome = false;
            }
            break;
        }
    }

    if (isPlayerHome) {
        careerSettings.logoId = career->logoId;
        careerSettings.pitch = career->homePitch;
    } else {
        for (const auto& team : career->leagueTable) {
            if (team.id == careerSettings.opponentTeamId) {
                careerSettings.logoId = team.logoId;
                careerSettings.pitch = team.homePitch;
                break;
            }
        }
    }

    careerSettings.weather = WeatherType::Clear;
    careerSettings.difficulty = Difficulty::Medium;

    game->changeState(std::make_unique<MatchState>(game, careerSettings));
}


void CareerHubState::simulateNextMatch() {
    int oppId = -1;

    for (const auto& match : career->schedule) {
        if (match.week == career->currentWeek) {
            if (match.homeTeamId == 0) {
                oppId = match.awayTeamId;
                break;
            }
            else if (match.awayTeamId == 0) {
                oppId = match.homeTeamId;
                break;
            }
        }
    }

    if (oppId != -1) {
        float playerPower = 0.f;
        int starterCount = 0;
        for (const auto& p : career->roster) {
            if (p.isStarter) {
                playerPower += (p.stats.speed + p.stats.shooting + p.stats.passing + p.stats.tackling) / 4.0f;
                starterCount++;
            }
        }
        if (starterCount > 0) playerPower /= starterCount;

        float oppPower = career->leagueTable[oppId].teamAverageStats.speed;

        int playerScore = rand() % 4;
        int oppScore = rand() % 4;

        if (playerPower > oppPower + 10.f) playerScore += 1 + (rand() % 2);
        else if (oppPower > playerPower + 10.f) oppScore += 1 + (rand() % 2);

        game->changeState(std::make_unique<PostMatchState>(game, career, playerScore, oppScore, oppId));
    }
}