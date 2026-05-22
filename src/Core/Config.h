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
}