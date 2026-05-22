#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class PauseAction {
    None,
    Resume,
    Restart,
    Quit
};

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

    PauseAction handleInput(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderTarget& target);
};