#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../Entities/GameObject.h"

class HUD {
private:
    sf::Font font;
    sf::Text scoreText;

public:
    HUD();

    void updateScore(int homeScore, int awayScore);

    void render(sf::RenderTarget& target, const std::vector<std::unique_ptr<GameObject>>& gameObjects) const;
};