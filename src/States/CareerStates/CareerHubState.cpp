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

CareerHubState::CareerHubState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), headerText(font), statsText(font), selectedIndex(0)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for CareerHub!\n";
    }

    // 1. Team Name Header
    headerText.setString(career->teamName + " - Week " + std::to_string(career->currentWeek));
    headerText.setCharacterSize(60);
    headerText.setFillColor(sf::Color::Yellow);
    headerText.setPosition({100.f, 50.f}); // Top left corner

    // 2. Resources Info
    statsText.setString("Street Cred: $" + std::to_string(career->streetCred));
    statsText.setCharacterSize(40);
    statsText.setFillColor(sf::Color::Green);
    statsText.setPosition({100.f, 120.f});

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
    // 3. Setup the Hub Options
    std::vector<std::string> optionsText = {
        "Play Next Match vs " + opponentName,
        "My Crew (Roster)",
        "The Block (Shop)",
        "League Standings",
        "Top Scorer Standings",
        "Save & Quit to Main Menu"
    };

    for (int i = 0; i < optionsText.size(); ++i) {
        sf::Text option(font);
        option.setString(optionsText[i]);
        option.setCharacterSize(50);

        // Aligning these to the left side of the screen like a dashboard menu
        option.setPosition({100.f, 300.f + (i * 80.f)});
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

            // if (selectedIndex == 0) { // "Play Next Match"
            //
            //
            //
            //     MatchSettings careerSettings;
            //     careerSettings.teamSize = 3;
            //     careerSettings.homeHumans = 1;
            //     careerSettings.awayHumans = 0;
            //     careerSettings.careerSave = career;
            //
            //     for (const auto& match : career->schedule) {
            //         if (match.week == career->currentWeek) {
            //             if (match.homeTeamId == 0) careerSettings.opponentTeamId = match.awayTeamId;
            //             else if (match.awayTeamId == 0) careerSettings.opponentTeamId = match.homeTeamId;
            //             break;
            //         }
            //     }
            //
            //     careerSettings.pitch = PitchType::Asphalt;
            //     careerSettings.weather = WeatherType::Clear;
            //     careerSettings.difficulty = Difficulty::Medium;
            //
            //     if (careerSettings.opponentTeamId != -1) {
            //         for (const auto& team : career->leagueTable) {
            //             if (team.id == careerSettings.opponentTeamId) {
            //                 careerSettings.logoId = team.logoId;
            //                 careerSettings.pitch = team.homePitch;
            //                 break;
            //             }
            //         }
            //     }
            //
            //     game->changeState(std::make_unique<MatchState>(game, careerSettings));
            // }
            else if (selectedIndex == 1) { // "My Crew"
                game->changeState(std::make_unique<ClubState>(game, career));
            }
            else if (selectedIndex == 2) { // "The Block"
                game->changeState(std::make_unique<ShopState>(game, career));
            }
            else if (selectedIndex == 3) { // Assuming index 3 is "League Standings"
                game->changeState(std::make_unique<StandingsState>(game, career));
            }
            else if (selectedIndex == 4) {
                game->changeState(std::make_unique<TopScorersState>(game, career));
            }
            else if (selectedIndex == 5) {
                // TODO: Save to file here!
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
    target.draw(headerText);
    target.draw(statsText);
    for (auto& option : menuOptions) {
        target.draw(option);
    }
}