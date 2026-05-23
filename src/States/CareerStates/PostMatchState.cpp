#include "PostMatchState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include "../../Career/LeagueSimulator.h"
#include <iostream>
#include <fstream>
#include <string>

PostMatchState::PostMatchState(Game* game, std::shared_ptr<CareerData> careerData, int homeScore, int awayScore)
    : GameState(game), career(careerData), matchHomeScore(homeScore), matchAwayScore(awayScore), credEarned(0),
      titleText(font), scoreText(font), rewardText(font), promptText(font)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for PostMatchState!\n";
    }

    // 1. Process the Match Results & Auto-Save
    calculateRewards();

    // 2. Setup Title Text
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
            match.homeScore = matchHomeScore;
            match.awayScore = matchAwayScore;
            break;
        }
    }

    LeagueSimulator::simulateWeek(career, career->currentWeek);

    career->currentWeek += 1;

    saveCareer();
}

void PostMatchState::saveCareer() {
    // A simple text-based save file to persist your data!
    std::ofstream saveFile("career_save.txt");
    if (saveFile.is_open()) {
        saveFile << career->teamName << "\n";
        saveFile << career->streetCred << "\n";
        saveFile << career->currentWeek << "\n";
        // Later down the line, we will loop through career->roster and save their exact stats here too!
        saveFile.close();
        std::cout << "Game Saved Successfully to career_save.txt!\n";
    } else {
        std::cerr << "Error: Could not save game!\n";
    }
}

void PostMatchState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {
            // Boot back to the Hub, passing the updated career data!
            game->changeState(std::make_unique<CareerHubState>(game, career));
        }
    }
}

void PostMatchState::update(float dt) {
    // The state is mostly static, no complex updates needed here for now
}

void PostMatchState::render(sf::RenderTarget& target) {
    target.draw(titleText);
    target.draw(scoreText);
    target.draw(rewardText);
    target.draw(promptText);
}