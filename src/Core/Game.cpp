#include "Game.h"
#include "../States/MatchState.h"
#include "Config.h"
#include "../States/MenuState.h"

Game::Game() {
    window.create(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}),
        "Street Football Manager",
        sf::Style::Default
        // ,sf::State::Fullscreen
        );
    window.setFramerateLimit(60);

    Config::loadSettings();

    audioManager = std::make_unique<AudioManager>();

    audioManager->loadSound("whistle", "assets/music/whistle.wav");
    audioManager->loadSound("tackle", "assets/music/thud.wav");
    audioManager->loadSound("goal", "assets/music/goalCheer.wav");

    std::vector<std::string> myTracks = {
        "assets/music/song1.mp3",
        "assets/music/song2.mp3",
        "assets/music/song3.mp3",
        "assets/music/song4.mp3",
        "assets/music/song5.mp3",
        "assets/music/song6.mp3"
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
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

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