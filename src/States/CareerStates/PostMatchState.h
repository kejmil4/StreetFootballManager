#pragma once
#include "../GameState.h"
#include "../../Career/CareerData.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <memory>

class PostMatchState : public GameState {
private:
    std::shared_ptr<CareerData> career;
    int matchHomeScore;
    int matchAwayScore;
    int credEarned;
    int opponentTeamId;

    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text rewardText;
    sf::Text promptText;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    void calculateRewards();
    void saveCareer();

public:
    PostMatchState(Game* game, std::shared_ptr<CareerData> careerData, int homeScore, int awayScore, int oppId);
    ~PostMatchState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};