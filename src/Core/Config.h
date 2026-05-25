#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <fstream>
#include <iostream>

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

    // Volumes go from 0.0f to 100.0f
    inline float masterVolume = 100.f;
    inline float musicVolume  = 50.f;
    inline float sfxVolume    = 80.f;


    inline void saveSettings() {
        std::ofstream file("settings.txt");
        if (file.is_open()) {
            file << masterVolume << "\n";
            file << musicVolume << "\n";
            file << sfxVolume << "\n";

            file << static_cast<int>(p1Binds.up) << "\n";
            file << static_cast<int>(p1Binds.down) << "\n";
            file << static_cast<int>(p1Binds.left) << "\n";
            file << static_cast<int>(p1Binds.right) << "\n";
            file << static_cast<int>(p1Binds.passSwitch) << "\n";
            file << static_cast<int>(p1Binds.shootTackle) << "\n";
            file << static_cast<int>(p1Binds.lobModifier) << "\n";

            file << static_cast<int>(p2Binds.up) << "\n";
            file << static_cast<int>(p2Binds.down) << "\n";
            file << static_cast<int>(p2Binds.left) << "\n";
            file << static_cast<int>(p2Binds.right) << "\n";
            file << static_cast<int>(p2Binds.passSwitch) << "\n";
            file << static_cast<int>(p2Binds.shootTackle) << "\n";
            file << static_cast<int>(p2Binds.lobModifier) << "\n";

            file.close();
        } else {
            std::cerr << "Failed to save settings.txt!\n";
        }
    }

    inline void loadSettings() {
        std::ifstream file("settings.txt");

        if (file.is_open()) {
            file >> masterVolume;
            file >> musicVolume;
            file >> sfxVolume;

            int key;
            file >> key; p1Binds.up = static_cast<sf::Keyboard::Key>(key);
            file >> key; p1Binds.down = static_cast<sf::Keyboard::Key>(key);
            file >> key; p1Binds.left = static_cast<sf::Keyboard::Key>(key);
            file >> key; p1Binds.right = static_cast<sf::Keyboard::Key>(key);
            file >> key; p1Binds.passSwitch = static_cast<sf::Keyboard::Key>(key);
            file >> key; p1Binds.shootTackle = static_cast<sf::Keyboard::Key>(key);
            file >> key; p1Binds.lobModifier = static_cast<sf::Keyboard::Key>(key);

            file >> key; p2Binds.up = static_cast<sf::Keyboard::Key>(key);
            file >> key; p2Binds.down = static_cast<sf::Keyboard::Key>(key);
            file >> key; p2Binds.left = static_cast<sf::Keyboard::Key>(key);
            file >> key; p2Binds.right = static_cast<sf::Keyboard::Key>(key);
            file >> key; p2Binds.passSwitch = static_cast<sf::Keyboard::Key>(key);
            file >> key; p2Binds.shootTackle = static_cast<sf::Keyboard::Key>(key);
            file >> key; p2Binds.lobModifier = static_cast<sf::Keyboard::Key>(key);

            file.close();
        }
    }
}