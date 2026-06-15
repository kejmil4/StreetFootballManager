#pragma once
#include "GameState.h"
#include "MatchState.h"
#include <SFML/Graphics.hpp>

/**
 * @class GameOverState
 * @brief The post-match summary screen for standard Exhibition games.
 * Displays the final scoreline, calculates the winner, and waits for user input
 * to return to the main menu.
 */
class GameOverState : public GameState {
private:
    sf::Font font;
    sf::Text resultText;
    sf::Text scoreText;
    sf::Text promptText;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

public:
    GameOverState(Game* game, int homeScore, int awayScore);
    ~GameOverState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};