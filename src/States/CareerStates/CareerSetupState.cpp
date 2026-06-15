#include "CareerSetupState.h"
#include "CareerHubState.h"
#include "../../Career/LeagueSimulator.h"
#include "../../Core/Game.h"
#include "../../Core/Config.h"
#include <iostream>

CareerSetupState::CareerSetupState(Game* game)
    : GameState(game), selectedIndex(0), selectedPitch(0), selectedLogo(0), selectedArchetype(0), playerNameString("THE ROOKIES"), titleText(font), nameInputText(font), bgSprite(bgTexture)
{
    if (!font.openFromFile("assets/font.ttf")) {
        std::cerr << "FAILED TO LOAD: assets/font.ttf for GameOverState!\n";
    }

    std::string bgFilePath = "assets/menus/menuCareerSetup5.png";
    if (!bgTexture.loadFromFile(bgFilePath)) {
        std::cerr << "FAILED TO LOAD BG: " << bgFilePath;
    }
    bgSprite.setTexture(bgTexture, true);

    sf::Vector2u textureSize = bgTexture.getSize();
    float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
    bgSprite.setScale({scaleX, scaleY});


    nameInputText.setFont(font);
    nameInputText.setCharacterSize(20);
    nameInputText.setPosition({Config::WINDOW_WIDTH * 0.66f, Config::WINDOW_HEIGHT * 0.22f});

    for (int i = 0; i < 4; ++i) {
        sf::Text opt(font);
        opt.setCharacterSize(18);
        opt.setPosition({Config::WINDOW_WIDTH * 0.66f, Config::WINDOW_HEIGHT * 0.35f + (i * Config::WINDOW_HEIGHT * 0.13f)});
        menuOptions.push_back(opt);
    }

    career = std::make_shared<CareerData>();
    refreshUI();
}

void CareerSetupState::refreshUI() {
    // 1. Team Name Input
    nameInputText.setString(playerNameString + (selectedIndex == 0 ? "_" : ""));
    nameInputText.setFillColor(selectedIndex == 0 ? sf::Color::Cyan : sf::Color::White);

    // 2. Toggles
    std::vector<std::string> pitches = {"Grass", "Asphalt", "Mud"};
    std::vector<std::string> styles = {"Balanced", "Speedsters", "Bruisers"};
    std::vector<std::string> logos = {"Skull", "Crown", "Snake", "Lightning", "Star", "Cross"};

    menuOptions[0].setString("< Home Court: " + pitches[selectedPitch] + " >");
    menuOptions[1].setString("< Team Logo: " + logos[selectedLogo] + " >");
    menuOptions[2].setString("< Play Style: " + styles[selectedArchetype] + " >");
    menuOptions[3].setString("START CAREER");

    for (int i = 0; i < 4; ++i) {
        menuOptions[i].setFillColor((selectedIndex == i + 1) ? sf::Color::Cyan : sf::Color::White);
    }
}

void CareerSetupState::handleInput(const sf::Event& event) {
    if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
        if (selectedIndex == 0) {
            // Handle Backspace (ASCII 8)
            if (textEvent->unicode == 8 && !playerNameString.empty()) {
                playerNameString.pop_back();
            }
            // Handle normal characters (Letters, Numbers, Spaces) - Limit to 15 chars
            else if (textEvent->unicode >= 32 && textEvent->unicode <= 126 && playerNameString.length() < 15) {
                playerNameString += static_cast<char>(textEvent->unicode);
            }
            refreshUI();
            return;
        }
    }

    // --- MENU NAVIGATION LOGIC ---
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Up) {
            if (selectedIndex > 0) selectedIndex--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Down) {
            if (selectedIndex < 4) selectedIndex++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Left) {
            if (selectedIndex == 1 && selectedPitch > 0) selectedPitch--;
            if (selectedIndex == 2 && selectedLogo > 0) selectedLogo--;
            if (selectedIndex == 3 && selectedArchetype > 0) selectedArchetype--;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Right) {
            if (selectedIndex == 1 && selectedPitch < 2) selectedPitch++;
            if (selectedIndex == 2 && selectedLogo < 5) selectedLogo++;
            if (selectedIndex == 3 && selectedArchetype < 2) selectedArchetype++;
        }
        else if (keyPressed->code == sf::Keyboard::Key::Enter && selectedIndex == 4) {

            career->teamName = playerNameString;
            career->streetCred = 500;
            career->currentWeek = 1;
            career->homePitch = static_cast<PitchType>(selectedPitch);
            career->logoId = selectedLogo + 12;

            // Generate Starters based on Archetype
            EntityStats baseline = {100.f, 100.f, 100.f, 100.f, 100.f};
            if (selectedArchetype == 1) { // Speedsters
                baseline.speed = 120.f; baseline.tackling = 70.f;
            } else if (selectedArchetype == 2) { // Bruisers
                baseline.tackling = 130.f; baseline.maxStamina = 130.f; baseline.speed = 80.f;
            }

            career->roster.push_back({"Captain", baseline, 300, true, 0});
            career->roster.push_back({"Rookie A", baseline, 200, true, 0});
            career->roster.push_back({"Rookie B", baseline, 200, true, 0});

            // Boot up the League Simulator!
            LeagueSimulator::initializeLeague(career, 8);

            game->changeState(std::make_unique<CareerHubState>(game, career));
        }
        refreshUI();
    }
}
void CareerSetupState::update(float dt) {}
void CareerSetupState::render(sf::RenderTarget& target) {
    target.draw(bgSprite);
    target.draw(nameInputText);
    for (const auto& opt : menuOptions) target.draw(opt);
}