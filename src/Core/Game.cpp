#include "Game.h"
#include "../States/GameState.h"

Game::Game() {
    // SFML 3.0 VideoMode constructor and state enum
    window.create(sf::VideoMode({800, 600}), "Street Football Manager", sf::Style::Default);
    window.setFramerateLimit(60);

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

    }
}

void Game::update(float dt) {

}

void Game::render() {
    window.clear(sf::Color::Black);

    window.display();
}