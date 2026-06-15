#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * @class AudioSettingsState
 * @brief UI screen for configuring global volume levels.
 * Allows the user to adjust Master, Music, and SFX volumes independently.
 * Changes are applied to the AudioManager in real-time for instant auditory
 * feedback, and saved to the local Config file upon exiting.
 */
class AudioSettingsState : public GameState {
private:
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    int selectedIndex;

    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    /**
     * @brief Updates the text strings to display the current volume percentages.
     */
    void refreshUI();

public:
    AudioSettingsState(Game* game);
    ~AudioSettingsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
};