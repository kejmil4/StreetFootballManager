#pragma once

namespace Config {
    constexpr unsigned int WINDOW_WIDTH = 1920;
    constexpr unsigned int WINDOW_HEIGHT = 1080;

    constexpr float CENTER_X = WINDOW_WIDTH / 2.0f;
    constexpr float CENTER_Y = WINDOW_HEIGHT / 2.0f;


    // --- NEW: PITCH PLAYABLE BOUNDARIES ---
    // Based on 480x270 scaled by 4x. (20px horizontal padding, 10px vertical)
    constexpr float PITCH_LEFT_X = 230.f;
    constexpr float PITCH_RIGHT_X = 1690.f;
    constexpr float PITCH_TOP_Y = 180.f;
    constexpr float PITCH_BOTTOM_Y = 880.f;

    // --- NEW: STREET GOAL DIMENSIONS ---
    // Based on a 64px high goal centered vertically
    constexpr float GOAL_TOP_Y = 415.f;
    constexpr float GOAL_BOTTOM_Y = 665.f;


    struct PlayerBinds {
        sf::Keyboard::Key up, down, left, right;
        sf::Keyboard::Key passSwitch;   // Offense: Pass | Defense: Switch Player
        sf::Keyboard::Key shootTackle;  // Offense: Shoot | Defense: Tackle
        sf::Keyboard::Key lobModifier;  // Modifies pass/shoot into a lob/chip
    };

    // Player 1 Defaults (WASD, E, Space, LShift)
    inline PlayerBinds p1Binds = {
        sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::A, sf::Keyboard::Key::D,
        sf::Keyboard::Key::E, sf::Keyboard::Key::Space, sf::Keyboard::Key::LShift
    };

    // Player 2 Defaults (Arrows, Period, Enter, RShift)
    inline PlayerBinds p2Binds = {
        sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, sf::Keyboard::Key::Left, sf::Keyboard::Key::Right,
        sf::Keyboard::Key::Period, sf::Keyboard::Key::Enter, sf::Keyboard::Key::RShift
    };
}