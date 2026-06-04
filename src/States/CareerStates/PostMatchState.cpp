#include "PostMatchState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include "../../Career/LeagueSimulator.h"
#include "CareerOverState.h"
#include <iostream>
#include <fstream>
#include <string>

PostMatchState::PostMatchState(Game* game, std::shared_ptr<CareerData> careerData, int homeScore, int awayScore, int oppId)
    : GameState(game), career(careerData), matchHomeScore(homeScore), matchAwayScore(awayScore), opponentTeamId(oppId), credEarned(0),
      titleText(font), scoreText(font), rewardText(font), promptText(font), bgSprite(bgTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for PostMatchState!\n";
    }

    std::string bgFilePath = "assets/menus/menuMatchResults.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath;
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});

    // Process the Match Results & Auto-Save
    calculateRewards();

    std::string titleStr;
    sf::Color titleColor;
    if (homeScore > awayScore) {
        titleStr = "VICTORY!";
        titleColor = sf::Color::Green;
    } else if (homeScore < awayScore) {
        titleStr = "DEFEAT!";
        titleColor = sf::Color::Red;
    } else {
        titleStr = "DRAW!";
        titleColor = sf::Color::Yellow;
    }

    titleText.setString(titleStr);
    titleText.setCharacterSize(100);
    titleText.setFillColor(titleColor);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    titleText.setPosition({Config::CENTER_X, Config::CENTER_Y - 200.f});

    // 3. Setup Score Text
    scoreText.setString("Final Score: " + std::to_string(homeScore) + " - " + std::to_string(awayScore));
    scoreText.setCharacterSize(60);
    scoreText.setFillColor(sf::Color::White);
    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin({scoreBounds.size.x / 2.0f, scoreBounds.size.y / 2.0f});
    scoreText.setPosition({Config::CENTER_X, Config::CENTER_Y - 50.f});

    // 4. Setup Reward Text
    rewardText.setString("Street Cred Earned: +$" + std::to_string(credEarned));
    rewardText.setCharacterSize(50);
    rewardText.setFillColor(sf::Color::Green);
    sf::FloatRect rewardBounds = rewardText.getLocalBounds();
    rewardText.setOrigin({rewardBounds.size.x / 2.0f, rewardBounds.size.y / 2.0f});
    rewardText.setPosition({Config::CENTER_X, Config::CENTER_Y + 100.f});

    // 5. Setup Prompt Text
    promptText.setString("Press ENTER to return to The Block");
    promptText.setCharacterSize(40);
    promptText.setFillColor(sf::Color::Cyan);
    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin({promptBounds.size.x / 2.0f, promptBounds.size.y / 2.0f});
    promptText.setPosition({Config::CENTER_X, Config::CENTER_Y + 250.f});
}


void PostMatchState::calculateRewards() {
    credEarned = 50;
    if (matchHomeScore > matchAwayScore) credEarned += 150;
    else if (matchHomeScore == matchAwayScore) credEarned += 50;

    career->streetCred += credEarned;

    LeagueTeam* myTeam = nullptr;
    for (auto& team : career->leagueTable) {
        if (team.id == 0) {
            myTeam = &team;
            break;
        }
    }

    if (myTeam) {
        myTeam->goalsFor += matchHomeScore;
        myTeam->goalsAgainst += matchAwayScore;

        if (matchHomeScore > matchAwayScore) {
            myTeam->points += 3;
            myTeam->wins++;
        } else if (matchHomeScore < matchAwayScore) {
            myTeam->losses++;
        } else {
            myTeam->points += 1;
            myTeam->draws++;
        }
    }

    for (auto& match : career->schedule) {
        if (match.week == career->currentWeek && (match.homeTeamId == 0 || match.awayTeamId == 0)) {
            match.isPlayed = true;
            if (match.homeTeamId == 0) {
                match.homeScore = matchHomeScore; // Player is home
                match.awayScore = matchAwayScore; // AI is away
            } else {
                match.homeScore = matchAwayScore; // AI is home
                match.awayScore = matchHomeScore; // Player is away
            }
            break;
        }
    }
    LeagueSimulator::assignGoals(career->roster, matchHomeScore);


    for (auto& team : career->leagueTable) {
        if (team.id == opponentTeamId) {
            LeagueSimulator::assignGoals(team.roster, matchAwayScore);
            break;
        }
    }
    LeagueSimulator::simulateWeek(career, career->currentWeek);
    LeagueSimulator::sortStandings(career);

    career->currentWeek += 1;

    if (career->currentWeek > 14) {

    } else {
        career->saveToFile();
    }

}


void PostMatchState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {

            if (career->currentWeek > 14) {
                game->changeState(std::make_unique<CareerOverState>(game, career));
            } else {
                game->changeState(std::make_unique<CareerHubState>(game, career));
            }
        }
    }
}

void PostMatchState::update(float dt) {
}

void PostMatchState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    target.draw(titleText);
    target.draw(scoreText);
    target.draw(rewardText);
    target.draw(promptText);
}