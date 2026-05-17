#pragma once

namespace Config {
    constexpr unsigned int WINDOW_WIDTH = 1920;
    constexpr unsigned int WINDOW_HEIGHT = 1080;

    constexpr float CENTER_X = WINDOW_WIDTH / 2.0f;
    constexpr float CENTER_Y = WINDOW_HEIGHT / 2.0f;


    // --- NEW: PITCH PLAYABLE BOUNDARIES ---
    // Based on 480x270 scaled by 4x. (20px horizontal padding, 10px vertical)
    constexpr float PITCH_LEFT_X = 80.f;
    constexpr float PITCH_RIGHT_X = 1840.f;
    constexpr float PITCH_TOP_Y = 40.f;
    constexpr float PITCH_BOTTOM_Y = 1040.f;

    // --- NEW: STREET GOAL DIMENSIONS ---
    // Based on a 64px high goal centered vertically
    constexpr float GOAL_TOP_Y = 412.f;    // 103px * 4
    constexpr float GOAL_BOTTOM_Y = 668.f; // 167px * 4
}