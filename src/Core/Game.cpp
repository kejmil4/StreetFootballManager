#include "Game.h"
#include "../States/MatchState.h"
#include "Config.h"

Game::Game() {
    // SFML 3.0 VideoMode constructor and state enum
    window.create(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Street Football Manager", sf::Style::Default);
    window.setFramerateLimit(60);

    currentState = std::make_unique<MatchState>();

}

Game::~Game() = default;

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        processEvents();
        update(dt);
        render();
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
    // ADD THIS: Tell the current state to update all its objects
    if (currentState) {
        currentState->update(dt);
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    // ADD THIS: Tell the current state to draw all its objects to the window
    if (currentState) {
        currentState->render(window);
    }

    window.display();
}