#pragma once
#include "GameState.h"
#include "MatchState.h"
#include <SFML/Graphics.hpp>

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