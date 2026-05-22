#pragma once
#include "GameState.h"
#include "MatchState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

class GameOverState : public GameState {
private:
    sf::Font font;
    sf::Text resultText;
    sf::Text scoreText;
    sf::Text promptText;

public:
    GameOverState(Game* game, int homeScore, int awayScore);
    ~GameOverState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};