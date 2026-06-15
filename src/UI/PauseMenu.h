#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * @enum PauseAction
 * @brief Represents the command the user selected in the pause menu.
 * By returning an enum rather than executing the logic here, we decouple
 * the UI from the core engine, allowing the MatchState to handle the actual
 * state transitions safely.
 */
enum class PauseAction {
    None,
    Resume,
    Restart,
    Quit
};

/**
 * @class PauseMenu
 * @brief A lightweight UI overlay that interrupts the active MatchState.
 * It does not inherit from GameState because it is not a full screen;
 * rather, it is drawn directly on top of the frozen MatchState to provide
 * context to the user that the game is paused.
 */

class PauseMenu {
private:
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    int selectedIndex;

    sf::RectangleShape backgroundOverlay;

public:
    PauseMenu();
    ~PauseMenu() = default;

    /**
     * @brief Processes keyboard input specifically for menu navigation.
     * @return The specific action requested by the user, or 'None' if just navigating.
     */
    PauseAction handleInput(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderTarget& target);
};