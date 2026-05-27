#include "ClubState.h"
#include "CareerHubState.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>

ClubState::ClubState(Game* game, std::shared_ptr<CareerData> careerData)
    : GameState(game), career(careerData), titleText(font), instructionText(font), warningText(font), selectedIndex(0), exitText(font), bgSprite(bgTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for ClubState!\n";
    }

    std::string bgFilePath = "assets/menus/menuRoster.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath;
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});


    instructionText.setString("Press ENTER to toggle Starter/Bench. Max 3 Starters.");
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition({Config::WINDOW_WIDTH * 0.18f, Config::WINDOW_HEIGHT * 0.3f});

    warningText.setCharacterSize(20);
    warningText.setFillColor(sf::Color::Red);
    warningText.setPosition({Config::WINDOW_WIDTH * 0.18f, Config::WINDOW_HEIGHT * 0.35f});
    warningText.setString("");

    exitText.setFont(font);
    exitText.setString("Back to Hub");
    exitText.setCharacterSize(20);
    exitText.setPosition({Config::WINDOW_WIDTH * 0.18f, Config::WINDOW_HEIGHT * 0.8f});

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
    float startX = Config::WINDOW_WIDTH * 0.3f;
    float startY = Config::WINDOW_HEIGHT * 0.42f;
    float spacingY = Config::WINDOW_HEIGHT * 0.05f;


    for (int i = 0; i < career->roster.size(); ++i) {
        sf::Text txt(font);
        CareerPlayer& p = career->roster[i];

        //Display the players' crew (roster) and their stats

        std::string status = p.isStarter ? "[STARTER]" : "[BENCH]";
        std::string info = status + " " + p.name + "\nSpd:" + std::to_string((int)p.stats.speed) +
                           " Sht:" + std::to_string((int)p.stats.shooting) + " Pass:" + std::to_string((int)p.stats.passing)
                            + " Tack:" + std::to_string((int)p.stats.tackling) + " Stamina:" + std::to_string((int)p.stats.maxStamina);

        txt.setString(info);
        txt.setCharacterSize(20);
        txt.setPosition({startX, startY + (static_cast<float>(i) * spacingY)});

        // Display in green if they are a started
        if (p.isStarter) txt.setFillColor(sf::Color::Green);
        else txt.setFillColor(sf::Color(150, 150, 150)); // Gray for benched

        rosterTexts.push_back(txt);
    }
}

void ClubState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        warningText.setString("");
        int maxIndex = career->roster.size();

        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < maxIndex) selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {

            // If the user clicked a player
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
            // If the user clicked "Exit"
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
    target.draw(bgSprite);
    target.draw(instructionText);
    target.draw(warningText);

    for (const auto& txt : rosterTexts) {
        target.draw(txt);
    }
    target.draw(exitText);
}