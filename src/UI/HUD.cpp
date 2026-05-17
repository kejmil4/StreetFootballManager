#include "HUD.h"
#include "../Entities/Footballer.h"
#include "../Core/Config.h"
#include <iostream>

#include "Entities/Footballer.h"

HUD::HUD() : font(), scoreText(font), timerText(font){
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "ERROR: Could not load assets/font.ttf!\n";
    }

    scoreText.setCharacterSize(28);
    timerText.setCharacterSize(28);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(3.f);

    updateScore(0, 0);
}

void HUD::updateScore(int homeScore, int awayScore) {
    scoreText.setString("     HOME " + std::to_string(homeScore) + " - " + std::to_string(awayScore) + " AWAY");

    scoreText.setOrigin({0.f, 0.f});

    scoreText.setPosition({200.f, 5.f});
}

void HUD::render(sf::RenderTarget& target, const std::vector<std::unique_ptr<GameObject>>& gameObjects) const {
    // 1. Draw the Scoreboard
    target.draw(scoreText);
    target.draw(timerText);

    // 2. Draw Stamina Bars for the Player(s)
    for (const auto& obj : gameObjects) {
        if (auto player = dynamic_cast<Footballer*>(obj.get())) {

            float currentStam = player->getStamina();
            float maxStam = player->getStats().maxStamina;
            float stamRatio = currentStam / maxStam;

            float barWidth = 60.f;
            float barHeight = 8.f;

            // Black Background Bar
            sf::RectangleShape bgBar({barWidth, barHeight});
            bgBar.setPosition({player->getPosition().x - (barWidth / 2.f), player->getPosition().y - 85.f});
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

void HUD::updateTimer(float secondsRemaining) {
    int minutes = static_cast<int>(secondsRemaining) / 60;
    int seconds = static_cast<int>(secondsRemaining) % 60;

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);

    timerText.setString(buffer);

    timerText.setOrigin({0.f, 0.f});


    timerText.setPosition({80.f, 5.f});


}