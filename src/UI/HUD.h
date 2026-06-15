#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../Entities/GameObject.h"

/**
 * @class HUD
 * @brief Manages the Heads-Up Display (UI Overlay) during a match.
 * Responsible for rendering global match data (Score and Time) as well as
 * entity-specific data (Stamina bars and Human-Controller indicator arrows).
 * It sits at the absolute top of the Z-order so it is never obscured by players or weather.
 */

class HUD {
private:
    sf::Font font;
    sf::Text scoreText;

    sf::Text timerText;

public:
    HUD();

    void updateScore(int homeScore, int awayScore);

    void render(sf::RenderTarget& target, const std::vector<std::unique_ptr<GameObject>>& gameObjects) const;

    void updateTimer(float secondsRemaining);
};