#include "HUD.h"
#include "../Entities/PlayerControlled.h"
#include "../Core/Config.h"
#include <iostream>

HUD::HUD() : font(), scoreText(font){
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "ERROR: Could not load assets/font.ttf!\n";
    }

    scoreText.setCharacterSize(48);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(3.f);

    updateScore(0, 0);
}

void HUD::updateScore(int homeScore, int awayScore) {
    scoreText.setString("HOME  " + std::to_string(homeScore) + "  -  " + std::to_string(awayScore) + "  AWAY");

    // Re-center the text just in case the score gets wider (e.g., 10-10)
    sf::FloatRect textBounds = scoreText.getLocalBounds();
    scoreText.setOrigin({textBounds.size.x / 2.0f, 0.f});
    scoreText.setPosition({Config::CENTER_X, 20.f});
}

void HUD::render(sf::RenderTarget& target, const std::vector<std::unique_ptr<GameObject>>& gameObjects) const {
    // 1. Draw the Scoreboard
    target.draw(scoreText);

    // 2. Draw Stamina Bars for the Player(s)
    for (const auto& obj : gameObjects) {
        if (auto player = dynamic_cast<PlayerControlled*>(obj.get())) {

            float currentStam = player->getStamina();
            float maxStam = player->getStats().maxStamina;
            float stamRatio = currentStam / maxStam;

            float barWidth = 40.f;
            float barHeight = 6.f;

            // Black Background Bar
            sf::RectangleShape bgBar({barWidth, barHeight});
            bgBar.setPosition({player->getPosition().x - (barWidth / 2.f), player->getPosition().y - 30.f});
            bgBar.setFillColor(sf::Color(0, 0, 0, 150));

            // Green Foreground Bar
            sf::RectangleShape fgBar({barWidth * stamRatio, barHeight});
            fgBar.setPosition(bgBar.getPosition());

            if (stamRatio < 0.3f) {
                fgBar.setFillColor(sf::Color::Red);
            } else {
                fgBar.setFillColor(sf::Color::Green);
            }

            target.draw(bgBar);
            target.draw(fgBar);
        }
    }
}