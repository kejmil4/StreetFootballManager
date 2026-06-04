#include "gameOverState.h"
#include "MenuState.h"
#include "../Core/Game.h"
#include "../Core/Config.h"
#include <iostream>
#include <string>

GameOverState::GameOverState(Game* game, int homeScore, int awayScore) : GameState(game), resultText(font), scoreText(font), promptText(font), bgSprite(bgTexture) {
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for GameOverState!\n";
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

    resultText.setFont(font);
    resultText.setString(resultStr);
    resultText.setCharacterSize(80);
    resultText.setFillColor(resultColor);

    sf::FloatRect resultBounds = resultText.getLocalBounds();
    resultText.setOrigin({resultBounds.size.x / 2.0f, resultBounds.size.y / 2.0f});
    resultText.setPosition({Config::CENTER_X, Config::CENTER_Y - 150.f});

    scoreText.setFont(font);
    scoreText.setString("Final Score: " + std::to_string(homeScore) + " - " + std::to_string(awayScore));
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);

    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin({scoreBounds.size.x / 2.0f, scoreBounds.size.y / 2.0f});
    scoreText.setPosition({Config::CENTER_X, Config::CENTER_Y});

    // Setup the Prompt Text
    promptText.setFont(font);
    promptText.setString("Press ENTER to return to Menu");
    promptText.setCharacterSize(25);
    promptText.setFillColor(sf::Color::Cyan);

    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin({promptBounds.size.x / 2.0f, promptBounds.size.y / 2.0f});
    promptText.setPosition({Config::CENTER_X, Config::CENTER_Y + 150.f});
}

void GameOverState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        // If the user presses Enter or Space, go back to the Main Menu
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space) {
            game->changeState(std::make_unique<MenuState>(game));
        }
    }
}

void GameOverState::update(float dt) {
}

void GameOverState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    target.draw(resultText);
    target.draw(scoreText);
    target.draw(promptText);
}