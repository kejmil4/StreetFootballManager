#include "Game.h"
#include "../States/MatchState.h"
#include "Config.h"
#include "../States/MenuState.h"

Game::Game() {
    window.create(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Street Football Manager", sf::Style::Default);
    window.setFramerateLimit(60);

    Config::loadSettings();

    audioManager = std::make_unique<AudioManager>();

    audioManager->loadSound("whistle", "assets/music/whistle.wav");
    audioManager->loadSound("tackle", "assets/music/thud.wav");
    audioManager->loadSound("goal", "assets/music/goalCheer.wav");

    std::vector<std::string> myTracks = {
        "assets/music/song1.mp3",
        "assets/music/song2.mp3"
    };
    audioManager->playPlaylist(myTracks, true);

    currentState = std::make_unique<MenuState>(this);

}

void Game::changeState(std::unique_ptr<GameState> newState) {
    nextState = std::move(newState);
}

Game::~Game() = default;

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        processEvents();
        update(dt);
        render();
        audioManager->update();
    }
}

void Game::processEvents() {
    // SFML 3.0 Event Polling (using std::optional)
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // Check if the event is a closed event
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        // ADD THIS: Pass the event down to the current state
        if (currentState) {
            currentState->handleInput(*event);
        }
    }
}

void Game::update(float dt) {
    if (nextState) {
        currentState = std::move(nextState);
    }

    if (currentState) {
        currentState->update(dt);
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    if (currentState) {
        currentState->render(window);
    }

    window.display();
}

void Game::closeApplication() {
    window.close();
}