#include "AudioSettingsState.h"
#include "MenuState.h"
#include "../Core/Game.h"
#include "../Core/Config.h"

AudioSettingsState::AudioSettingsState(Game* game) : GameState(game), selectedIndex(0), titleText(font), bgSprite(bgTexture) {
    font.openFromFile("assets/font.ttf");

    std::string bgFilePath = "assets/menus/menuAudioSettings.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath;
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});

    for (int i = 0; i < 4; ++i) {
        sf::Text text(font);
        text.setCharacterSize(45);
        text.setPosition({Config::WINDOW_WIDTH * 0.2f, 350.f + (i * 100.f)});
        menuOptions.push_back(text);
    }
    refreshUI();
}

void AudioSettingsState::refreshUI() {
    menuOptions[0].setString("Master Volume: < " + std::to_string((int)Config::masterVolume) + "% >");
    menuOptions[1].setString("Music Volume: < " + std::to_string((int)Config::musicVolume) + "% >");
    menuOptions[2].setString("SFX Volume: < " + std::to_string((int)Config::sfxVolume) + "% >");
    menuOptions[3].setString("Back to Menu");

    for (int i = 0; i < 4; ++i) {
        menuOptions[i].setFillColor(i == selectedIndex ? sf::Color::Cyan : sf::Color::White);
    }
}

void AudioSettingsState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {

        // Navigation
        if (keyPressed->code == sf::Keyboard::Key::Up && selectedIndex > 0) selectedIndex--;
        if (keyPressed->code == sf::Keyboard::Key::Down && selectedIndex < 3) selectedIndex++;

        // Left/Right to adjust volumes!
        if (keyPressed->code == sf::Keyboard::Key::Left) {
            if (selectedIndex == 0 && Config::masterVolume > 0.f) Config::masterVolume -= 5.f;
            if (selectedIndex == 1 && Config::musicVolume > 0.f)  Config::musicVolume -= 5.f;
            if (selectedIndex == 2 && Config::sfxVolume > 0.f)    Config::sfxVolume -= 5.f;

            game->getAudio()->updateVolumes();
        }
        if (keyPressed->code == sf::Keyboard::Key::Right) {
            if (selectedIndex == 0 && Config::masterVolume < 100.f) Config::masterVolume += 5.f;
            if (selectedIndex == 1 && Config::musicVolume < 100.f)  Config::musicVolume += 5.f;
            if (selectedIndex == 2 && Config::sfxVolume < 100.f)    Config::sfxVolume += 5.f;

            game->getAudio()->updateVolumes();
        }

        // Exit
        if (keyPressed->code == sf::Keyboard::Key::Enter && selectedIndex == 3) {
            Config::saveSettings();

            game->changeState(std::make_unique<MenuState>(game));
        }

        refreshUI();
    }
}
void AudioSettingsState::update(float dt) {}
void AudioSettingsState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    for (const auto& opt : menuOptions) target.draw(opt);
}