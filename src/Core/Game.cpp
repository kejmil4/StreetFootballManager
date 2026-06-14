#include "Game.h"
#include "../States/MatchState.h"
#include "Config.h"
#include "../States/MenuState.h"
#include <SFML/Graphics/Image.hpp>

Game::Game() {
    // --- 1. Window & Core Initialization ---
    window.create(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}),
        "Street Football Manager",
        sf::Style::Default
        );
    window.setFramerateLimit(60);

    // Load application taskbar/window icon
    sf::Image icon;
    if (icon.loadFromFile("assets/StreetFootballManagerLogo.png")) {
        window.setIcon(icon.getSize(), icon.getPixelsPtr());
    } else {
        std::cerr << "WARNING: Failed to load assets/icon.png for taskbar!\n";
    }

    // Load user preferences before initializing audio or states
    Config::loadSettings();

    // --- 2. Audio Subsystem Setup ---
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

    // --- 3. Initial State ---
    currentState = std::make_unique<MenuState>(this);

}

void Game::changeState(std::unique_ptr<GameState> newState) {
    nextState = std::move(newState);
}

Game::~Game() = default;

/**
 * The heartbeat of the application.
 * Continuously loops until the window is closed, driving events, logic, and rendering.
 */

void Game::run() {
    while (window.isOpen()) {
        // Calculate delta time (dt) for frame-rate independent movement/physics
        float dt = clock.restart().asSeconds();

        processEvents();
        update(dt);
        render();
        audioManager->update();
    }
}

/**
 * Polls the OS for window events (like clicking the 'X' button or keyboard presses)
 * and delegates raw input down to the active game state.
 */
void Game::processEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        // Intercept global quit commands
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        // Pass the event down the hierarchy
        if (currentState) {
            currentState->handleInput(*event);
        }
    }
}

void Game::update(float dt) {
    // Process queued state transitions before updating the active state.
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