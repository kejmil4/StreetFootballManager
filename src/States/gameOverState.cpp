#include "GameOverState.h"
#include "MenuState.h"
#include "../Core/Game.h"
#include "../Core/Config.h" // Needed for Config::CENTER_X / Y
#include <iostream>
#include <string>

GameOverState::GameOverState(Game* game, int homeScore, int awayScore) : GameState(game), resultText(font), scoreText(font), promptText(font) {
    // 1. Load the retro font
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for GameOverState!\n";
    }

    // 2. Determine Outcome and Colors
    std::string resultStr;
    sf::Color resultColor;

    if (homeScore > awayScore) {
        resultStr = "HOME TEAM WINS!";
        resultColor = sf::Color::Green;
    } else if (awayScore > homeScore) {
        resultStr = "AWAY TEAM WINS!";
        resultColor = sf::Color::Red;
    } else {
        resultStr = "IT'S A DRAW!";
        resultColor = sf::Color::Yellow;
    }

    // 3. Setup the Result Text (Top)
    resultText.setFont(font);
    resultText.setString(resultStr);
    resultText.setCharacterSize(80);
    resultText.setFillColor(resultColor);

    sf::FloatRect resultBounds = resultText.getLocalBounds();
    resultText.setOrigin({resultBounds.size.x / 2.0f, resultBounds.size.y / 2.0f});
    resultText.setPosition({Config::CENTER_X, Config::CENTER_Y - 150.f});

    // 4. Setup the Score Text (Middle)
    scoreText.setFont(font);
    scoreText.setString("Final Score: " + std::to_string(homeScore) + " - " + std::to_string(awayScore));
    scoreText.setCharacterSize(60);
    scoreText.setFillColor(sf::Color::White);

    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin({scoreBounds.size.x / 2.0f, scoreBounds.size.y / 2.0f});
    scoreText.setPosition({Config::CENTER_X, Config::CENTER_Y});

    // 5. Setup the Prompt Text (Bottom)
    promptText.setFont(font);
    promptText.setString("Press ENTER to return to Menu");
    promptText.setCharacterSize(40);
    promptText.setFillColor(sf::Color::Cyan);

    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin({promptBounds.size.x / 2.0f, promptBounds.size.y / 2.0f});
    promptText.setPosition({Config::CENTER_X, Config::CENTER_Y + 150.f});
}

void GameOverState::handleInput(const sf::Event& event) {
    // SFML 3.0 Syntax: Check for KeyPressed
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        // If they press Enter or Space, go back to the Main Menu
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {
            game->changeState(std::make_unique<MenuState>(game));
        }
    }
}

void GameOverState::update(float dt) {
    // You could add a retro blinking effect to promptText here using dt if you wanted,
    // but for now, it's fine to leave it empty since the state is mostly static.
}

void GameOverState::render(sf::RenderTarget& target) {
    target.draw(resultText);
    target.draw(scoreText);
    target.draw(promptText);
}